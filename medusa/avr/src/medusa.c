//============================================================================
// Name        : medusa.c
// Author      : Eric PASCUAL
// Version     :
// Copyright   : POBOT
// Description : Programme principal pour le contr�leur du radeau PSMed
// Target      : ATMega128
// Clock       : 16MHz
// Compiler    :avr-gcc (WinAVR)
//============================================================================

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <uart2.h>
#include <i2c.h>
#include <a2d.h>
#include <string.h>

#include "servo_ctl.h"
#include "gps_simple.h"
#include "onewire.h"
#include "ds18x20.h"
#include "delay.h"

#define UART_RF			0
#define UART_GPS		1

#define STX				0x02
#define ETX				0x03

// affectation des IOs

#define	TTL_OUT_PORT	PORTC
#define	TTL_OUT_DDR		DDRC

// D�sactivation de tous les relais
// Note : la carte relais est en logique n�gative, mais l'interface optique inverse les �tats
#define all_relays_off()	TTL_OUT_PORT = 0

#define ESC_PORT			PORTA
#define ESC_DDR				DDRA
#define ESC_MOTOR_1_NUM		0
#define ESC_MOTOR_2_NUM		1
#define ESC_WINCH_NUM		2

#define SYSTEM_FLAGS_PORT		PINE
#define SYSTEM_FLAGS_DDR		DDRE
#define WINCH_UPPER_LIMIT_BIT	4
#define WINCH_LOWER_LIMIT_BIT	5
#define WINCH_TICK_BIT			6
#define SYSTEM_FLAGS_BITS		(_BV(WINCH_UPPER_LIMIT_BIT) | _BV(WINCH_LOWER_LIMIT_BIT) | _BV(WINCH_TICK_BIT))

#define winch_is_full_up() 		bit_is_clear(SYSTEM_FLAGS_PORT, WINCH_UPPER_LIMIT_BIT)
#define winch_is_full_down() 	bit_is_clear(SYSTEM_FLAGS_PORT, WINCH_LOWER_LIMIT_BIT)

#define RF_POWER_SEL_PORT	PINB
#define RF_POWER_SEL_DDR	DDRB
#define RF_POWER_SEL_BIT	4

// ADC utilis�s pour le contr�le des batteries
#define ADC_BATT_LEVEL_LOGIC	ADC_CH_ADC6
#define ADC_BATT_LEVEL_POWER	ADC_CH_ADC7

// configuration des bus 1wire pour les sondes de temp�rature moteurs
#define MTEMP_IN		PINB
#define MTEMP_OUT		PORTB
#define MTEMP_DDR		DDRB
#define MTEMP_1_PIN		PB5
#define MTEMP_2_PIN		PB6

#define select_mtemp_1() ow_set_bus(&MTEMP_IN, &MTEMP_OUT, &MTEMP_DDR, MTEMP_1_PIN)
#define select_mtemp_2() ow_set_bus(&MTEMP_IN, &MTEMP_OUT, &MTEMP_DDR, MTEMP_2_PIN)

static char hex_digits[] = "0123456789ABCDEF";

/*
 * Conversion d'un digit hexa sous forme ASCII en sa valeur num�rique.
 * Si le caract�re est invalide, retourne 0xFF
 */
static u08 hex2u08(char c) {
	register char* p = strchr(hex_digits, c) ;
	return (p) ? p - hex_digits : 0xff;
}

static void delay_10ms(u08 cnt) {
	while (cnt) {
		_delay_ms(10) ;
		cnt--;
	}
}

// flag de d�clenchement de la collecte des donn�es
volatile u08 collect_data_request = TRUE ;

// indicateur d'�tat du processus de lecture des temp�ratures sur les DS18x20
enum DS1820_STATE {
	DSSTAT_IDLE,
	DSSTAT_START_CONV,
	DSSTAT_START_CONV_DONE,
	DSSTAT_READ_TEMP,
	DSSTAT_READ_TEMP_DONE
};
volatile u08 ds18x20_state = DSSTAT_IDLE ;

// delai entre la lecture de la temp�rature et la prochaine demande de conversion
#define DS1820_READ_TO_CONV_DELAY_SECS	9

u08 ds18x20_elapsed_secs = 0 ;

/* *************************************************************************************************
 * Timer 1 seconde
 *
 * Utilis� pour :
 *
 * 1/ la d�tection de perte de communication
 * Surveille qu'on re�oit r�guli�rement des trames depuis le poste de contr�le. Normalement,
 * celles-ci sont envoy�es toutes les 100ms. Si 1 seconde s'est �coul�e sans avoir re�u aucune trame,
 * on signale une anomalie.
 *
 * 2/ le s�quencement des �tapes de lecture des temp�ratures moteur via les DS18x20. La conversion
 * prend 750ms environ, donc on attend 1 seconde entre la commande de lancement de la conversion et
 * la lecture. On attend ensuite DS1820_READ_TO_CONV_DELAY_SECS secondes pour renvoyer la prochaine
 * commande de conversion, car la temp�rature �volue avec des constantes de temps assez longues,
 * donc pas la peine de se stresser
 *
 * Du fait du prescaler et du reset compteur utilis�s, l'interrupt est d�clench�e toutes les 8ms.
 * ************************************************************************************************* */

#define TCNT0_RESTART		(256 - 125)		// <=> OVF � 8ms
#define OVFCNT_1SEC			125				// <=> 1s

volatile u08 ovf8ms_cnt = 0 ;

volatile u08 rx_frame_cnt = 0 ;
volatile u08 communication_lost = 0 ;

SIGNAL(SIG_OVERFLOW0) {
	if(++ovf8ms_cnt == OVFCNT_1SEC) {
		// surveillance de la communication
		communication_lost = (rx_frame_cnt == 0) ;
		rx_frame_cnt = 0 ;

		// gestion des lectures DS18x20
		switch (ds18x20_state) {
		case DSSTAT_START_CONV_DONE :
			ds18x20_state = DSSTAT_READ_TEMP ;
			break ;
		case DSSTAT_READ_TEMP_DONE:
			if (++ds18x20_elapsed_secs == DS1820_READ_TO_CONV_DELAY_SECS) {
				ds18x20_state = DSSTAT_START_CONV ;
				ds18x20_elapsed_secs = 0 ;
			}
			break ;
		}

		ovf8ms_cnt = 0 ;
	}
	TCNT0 = TCNT0_RESTART ;
}

static void timer1sec_init(void) {
	TCCR0 = _BV(CS00) | _BV(CS01) | _BV(CS02) ;		// prescale = 1024
	TIMSK |= _BV(TOIE0) ;							// interrupt sur overflow TMR0
}

/* *************************************************************************************************
 * INITIALISATION GLOBALES
 * ************************************************************************************************* */

/*
 Initialisation des ports
 */
static void port_init(void) {
	// outputs TTL est configur� en sortie et relais d�sactiv�s
	TTL_OUT_DDR = 0xff;
	all_relays_off() ;

	// flags configur�s en entr�e
	SYSTEM_FLAGS_DDR &= ~SYSTEM_FLAGS_BITS;

	// s�lecteur de puissance RF configur� en entr�e
	RF_POWER_SEL_DDR &= ~_BV(RF_POWER_SEL_BIT);

	servo_port_init() ;
}

/*
 Initialisation des p�riph�riques
 */
static void init_devices(void) {
	// inhibition des interruptions (pour avoir la paix)
	cli() ;

	port_init(); // initialisation de ports
	servo_init_devices() ;

	timer1sec_init() ;

	// activation de l'INT6 pour les fronts descendants du capteur de mesure du treuil
	EICRB = _BV(ISC61) ;
	EIMSK |= _BV(INT6) ;

	// activation des interruptions
	sei
();
}

/* *************************************************************************************************
 * STRUCTURES DE DONNEES DU BACK-CHANNEL
 * ************************************************************************************************* */

#define TAG_EXTENDED	'X'
#define TAG_POSITION	'P'
#define TAG_SYSTEM		'S'
#define TAG_USER		'U'

typedef struct _FRAME_HEADER {
	u08 frame_start;
	u08 frame_tag;
	u08 payload_len;
} FRAME_HEADER;

#define TAG_FILLER		'*'
#define TAG_MSG			'M'
#define TAG_NONE		' '

#define TEMP_MOTOR_CNT	2

#define HW_COMPASS_BIT		0
#define HW_GPS_BIT			1
#define HW_REMOTE_SENS_BIT	2
#define HW_MOT1_TEMP_BIT	3
#define HW_MOT2_TEMP_BIT	4

// ADC en entr�e directe sur le MCU
#define LOCAL_SENSORS_CNT 	6
// ADC sur PCF8591
#define REMOTE_SENSORS_CNT 	4


// Allocation des diff�rentes trames du back-channel

// --------------------------------- position (GPS + cap)

struct _RF_POSITION_FRAME {
	FRAME_HEADER header ;
	POSITION_LL	pos_ll ;
	u16	heading ;
} rf_position_frame ;

// --------------------------------- infos syst�me

struct _RF_SYSTEM_FRAME {
	FRAME_HEADER header ;
	u08 battery_logic;
	u08 battery_power;
	u08 temp_motor[TEMP_MOTOR_CNT];
	u08 system_IOs;
	u08 hw_flags ;
	u16 winch_ticks ;	// impulsions de l'indicateur de d�roulement du c�ble
} rf_system_frame ;

#define compass_present (rf_system_frame.hw_flags & _BV(HW_COMPASS_BIT))
#define gps_present (rf_system_frame.hw_flags & _BV(HW_GPS_BIT))
#define remote_sensors_present (rf_system_frame.hw_flags & _BV(HW_REMOTE_SENS_BIT))
#define motor1_temp_present (rf_system_frame.hw_flags & _BV(HW_MOT1_TEMP_BIT))
#define motor2_temp_present (rf_system_frame.hw_flags & _BV(HW_MOT2_TEMP_BIT))
#define motor_temp_present (motor1_temp_present || motor2_temp_present)

// --------------------------------- donn�es utilisateur

struct _RF_USER_FRAME {
	FRAME_HEADER header ;
	u08 adc_local[LOCAL_SENSORS_CNT];
	u08 adc_remote[REMOTE_SENSORS_CNT];
} rf_user_frame ;

// --------------------------------- �tendue (position + syst�me + utilistaeur)

struct _RF_EXTENDED_FRAME {
	FRAME_HEADER header ;
	struct {
		POSITION_LL	pos_ll ;
		u16	heading ;
	} position ;
	struct {
		u08 battery_logic;
		u08 battery_power;
		u08 temp_motor[TEMP_MOTOR_CNT];
		u08 system_IOs;
		u08 hw_flags ;
		u16 winch_ticks ;
	} system ;
	struct {
		u08 adc_local[LOCAL_SENSORS_CNT];
		u08 adc_remote[REMOTE_SENSORS_CNT];
	} user ;
} rf_extended_frame ;

// --------------------------------- remplissage

struct _RF_FILLER_FRAME {
	FRAME_HEADER header ;
	u08 frame_cnt; // compteur s�quentiel � rebouclage
} rf_filler_frame;

// --------------------------------- message

#define MSG_LEN_MAX (UART0_TX_BUFFER_SIZE - sizeof(FRAME_HEADER) - 8)	// 8 = marge de s�curit�

struct _RF_MSG_FRAME {
	FRAME_HEADER frame_header;
	char msg[MSG_LEN_MAX];
} rf_msg_frame;

static void sendTextMessage(char *msg) {
	strncpy(rf_msg_frame.msg, msg, MSG_LEN_MAX) ;
	u08 len = strlen(msg) ;
	if (len > MSG_LEN_MAX)
		len = MSG_LEN_MAX ;
	rf_msg_frame.msg[len] = ETX ;							// ajoute la terminaison du message (ETX)
	rf_msg_frame.frame_header.payload_len = len + 1 ;		// payload_len inclut la terminaison
	uartSendBuffer(UART_RF, (char *)&rf_msg_frame, sizeof(rf_msg_frame));

	delay_10ms(5) ;
}

/* *************************************************************************************************
 * GESTION DES COMMANDES DU POSTE DE CONTROLE
 * ************************************************************************************************* */

typedef  enum _CMD_PROCESS_STATE {
	CMD_IDLE,
	CMD_RECEIVING,
	CMD_RECEIVED,
	CMD_PROCESSED
} CMD_PROCESS_STATE;

volatile CMD_PROCESS_STATE cmd_state = CMD_IDLE ;

/*
 * Bloc de commande envoy� par le poste de contr�le
 *
 * Le bloc de commande est de taille fixe et comprend syst�matiquement les consignes pour
 * les 3 ESC (moteurs de propulsion et moteur du treuil), ainsi que l'�tat des 8 IOs.
 * Ce bloc est encadr� par STX et ETX afin de constituer une trame compl�te. L'encodage
 * des donn�es contenues dans le bloc est d�fini de mani�re � ce qu'aucun des bytes de donn�e
 * ne puisse �tre confondu avec les caract�res de d�limitation de la trame.
 *
 * Une trame compl�te est d�finie sur 11 bytes, dont 3 pour le protocole. Elle est structur�e
 * comme suit :
 * <STX>
 * expected_reply
 * ESC0_dir ESC0_speed
 * ESC1_dir ESC1_speed
 * ESC2_dir ESC2_speed
 * IO7..4   IO3..0
 * <ETX>
 *
 * avec :
 * expected_reply = le code de la trame retour attendue, ou un espace si aucune r�ponse
 * ESCx_dir = direction moteur x ('+'=avant, '-'=arri�re, '0'=stop, '*'=disable)
 * ESCx_speed = vitesse moteur x ('0'..'9','A'..'F'), ignor�e si dir = '0'
 * IO7..4 = repr�sentation en hexa ASCII du nibble de poids fort de l'�tat des sorties ('0'..'F')
 * IO3..0 = idem pour le nibble de poids faible
 *
 * Hormis les bytes de protocole, la trame est donc visualisable directement.
 */

// bloc de contr�le d'un ESC (Electronic Speed Control)
typedef struct _ESC_CONTROL {
	char dir;
	char speed;
} ESC_CONTROL;

// nombre de pas pour le contr�le des ESC
#define ESC_RESOLUTION	0x04

// d�finition des ESC contr�l�s
enum _ESC {
	ESC_MIN,
	ESC_PROP_LEFT = ESC_MIN,
	ESC_PROP_RIGHT,
	ESC_WINCH,
	ESC_MAX = ESC_WINCH
};

struct _RC_CMDBLOCK {
	u08 expected_reply ;
	ESC_CONTROL esc[ESC_MAX - ESC_MIN + 1]; // contr�le des moteurs de propulsion et du treuil
	u08 ttl_out[2]; // �tat des sorties TTL
} rc_cmdblock;

u08 expected_reply = TAG_NONE ;

/* *************************************************************************************************
 * GESTION DU TREUIL
 * ************************************************************************************************* */

enum _WINCH_COMMAND {
	WINCH_CMD_DISABLE,
	WINCH_CMD_STOP,
	WINCH_CMD_WIND,
	WINCH_CMD_UP = WINCH_CMD_WIND,
	WINCH_CMD_UNWIND,
	WINCH_CMD_DOWN = WINCH_CMD_UNWIND,
};

u08 winch_command = WINCH_CMD_DISABLE ;
u08 winch_speed = 0;

/*
 * Mesure de la longueur du c�ble
 *
 * ATTENTION : si la pin du signal du capteur est chang�e, modifier l'interrupt en cons�quence
 */

SIGNAL(SIG_INTERRUPT6) {
//#define DEBOUNCE_LG
#ifdef DEBOUNCE_LG
	static u08 last_ovf8ms_cnt = 0 ;

	// filtre anti-rebond impl�ment� par la prise en compte du front uniquement si 8ms au moins apr�s le pr�c�dent
	if (ovf8ms_cnt != last_ovf8ms_cnt) {
#endif
		switch(winch_command) {
		case WINCH_CMD_WIND:
			--rf_system_frame.winch_ticks ;
			break ;
		case WINCH_CMD_UNWIND:
			++rf_system_frame.winch_ticks ;
			break ;
		}
#ifdef DEBOUNCE_LG
		last_ovf8ms_cnt = ovf8ms_cnt ;
	}
#endif
}

/*
 * Prise en compte des commandes du treuil
 */
inline static void set_winch_command(u08 command, u08 speed) {
	winch_command = command ;
	winch_speed = ((command == WINCH_CMD_WIND) || (command == WINCH_CMD_UNWIND)) ? speed : 0 ;
}

/*
 * Contr�le du treuil avec gestion des s�curit�s de fin de course
 */

static void winch_control(void) {
	switch (winch_command) {
	case WINCH_CMD_WIND :
		if (!winch_is_full_up()) {
			servo_set(ESC_WINCH_NUM, SERVO_POSITION_MED + winch_speed * ESC_RESOLUTION);
			servo_enable(ESC_WINCH_NUM) ;

		} else {
			set_winch_command(WINCH_CMD_STOP, 0) ;
			// remise � 0 du compteur de d�roulement de c�ble
			rf_system_frame.winch_ticks = 0 ;
		}
		break;

	case WINCH_CMD_UNWIND :
		if (!winch_is_full_down()) {
			servo_set(ESC_WINCH_NUM, SERVO_POSITION_MED - winch_speed * ESC_RESOLUTION);
			servo_enable(ESC_WINCH_NUM) ;

		} else {
			set_winch_command(WINCH_CMD_STOP, 0) ;
		}
		break;

	case WINCH_CMD_STOP :
		servo_set(ESC_WINCH_NUM, SERVO_POSITION_MED);
		servo_enable(ESC_WINCH_NUM) ;
		break ;

	default:
		servo_disable(ESC_WINCH_NUM) ;
	}
}

/* *************************************************************************************************
 * GESTION DES MOTEURS DE PROPULSION
 * ************************************************************************************************* */

inline static void motor_forward(u08 motorId, u08 speed) {
	if (!communication_lost) {
		servo_set(motorId, SERVO_POSITION_MED + speed * ESC_RESOLUTION);
	}
}

inline static void motor_reverse(u08 motorId, u08 speed) {
	if (!communication_lost) {
		servo_set(motorId, SERVO_POSITION_MED - speed * ESC_RESOLUTION);
	}
}

inline static void motor_stop(u08 motorId) {
	servo_set(motorId, SERVO_POSITION_MED);
}

inline static void motor_disable_all(void) {
	servo_disable(ESC_MOTOR_1_NUM) ;
	servo_disable(ESC_MOTOR_2_NUM) ;
}

inline static void motor_enable_all(void) {
	servo_enable(ESC_MOTOR_1_NUM) ;
	servo_enable(ESC_MOTOR_2_NUM) ;
}

/* *************************************************************************************************
 * Traitement d'un bloc de commande en provenance du poste de contr�le
 * ************************************************************************************************* */

static void process_rcblock(void) {
	/*
	 * Traitement des consignes pour les moteurs
	 */

	expected_reply = rc_cmdblock.expected_reply ;

	register u08 esc_id;
	for (esc_id = ESC_MIN ; esc_id <= ESC_MAX ; esc_id++) {
		u08 dir = rc_cmdblock.esc[esc_id].dir;
		if (dir == '0') {
			if (esc_id == ESC_WINCH_NUM) {
				set_winch_command(WINCH_CMD_STOP, 0);
			} else {
				servo_set(esc_id, SERVO_POSITION_MED);
			}

		} else {
			u08 speed = hex2u08(rc_cmdblock.esc[esc_id].speed) ;
			if (speed != 0xff) {
				switch (dir) {
				case '+':
					if (esc_id == ESC_WINCH_NUM) {
						set_winch_command(WINCH_CMD_WIND, hex2u08(rc_cmdblock.esc[esc_id].speed));
					} else {
						motor_forward(esc_id, hex2u08(rc_cmdblock.esc[esc_id].speed)) ;
					}
					break;
				case '-':
					if (esc_id == ESC_WINCH_NUM) {
						set_winch_command(WINCH_CMD_UNWIND, hex2u08(rc_cmdblock.esc[esc_id].speed));
					} else {
						motor_reverse(esc_id, hex2u08(rc_cmdblock.esc[esc_id].speed)) ;
					}
					break;
				}
			}
		}
	}

	/*
	 * Traitement des sorties TTL (logique positive)
	 */

	TTL_OUT_PORT = hex2u08(rc_cmdblock.ttl_out[0]) << 4 | hex2u08(rc_cmdblock.ttl_out[1]);

	cmd_state = CMD_PROCESSED;
}

/* *************************************************************************************************
 * GESTION DU GPS
 * ************************************************************************************************* */

// buffer de r�ception des messages du GPS
// (la taille correspond � la partie qui nous int�resse, l'exc�dent �tant ignor� � la r�ception)
#define GPS_RX_BUFFER_SIZE 	75
char gps_buffer[GPS_RX_BUFFER_SIZE] = "GPRMC,hhmmss,A,0000.0000,N,00000.0000,E,.";

char *gps_buffer_tail = gps_buffer ;
u08 gps_data_len = 0;
volatile u08 gps_data_available = FALSE;

/*
 * Interrupt handler de l'UART assign� au GPS
 */
static void GPSuartRxHandler(unsigned char c) {
	static u08 termCount = 0;

	rf_system_frame.hw_flags |= _BV(HW_GPS_BIT) ;

	switch (c) {
	case '$':
		gps_data_available = FALSE ;	// interdit l'acc�s au buffer pendant la r�ception d'une trame
		gps_buffer_tail = gps_buffer ;
		gps_data_len = 0;
		termCount = 0;
		break;

	case 0x0d:
	case 0x0a:
		*gps_buffer_tail++ = 0 ;
		gps_data_available = (++termCount == 2);
		break;

	default:
		if (gps_data_len < GPS_RX_BUFFER_SIZE) {
			*gps_buffer_tail++ = c ;
			gps_data_len++;
		}
	}
}

/*
 * Utilitaire d'envoi d'une commande au GPS
 */
static void sendGPSCommand(char *pCmde) {
	static char frame[32];

	register char *pOut = frame ;
	register u08 cs = 0;

	*pOut++ = '$';

	for (; *pCmde ; pCmde++) {
		*pOut++ = *pCmde ;
		cs ^= *pCmde ;
	}

	*pOut++ = '*';
	*pOut++ = hex_digits[(cs >> 4) & 0x0f];
	*pOut++ = hex_digits[cs & 0x0f];

	*pOut++ = 0x0d;
	*pOut++ = 0x0a;
	*pOut = 0;

	u08 len = pOut - frame ;

	uartSendBuffer(UART_GPS, frame, len);

	// laisse le temps � la transmission de se terminer
	delay_10ms(20) ;
}

/*
 * Initialisation du GPS
 */

static void setupGPS(void) {
	uartSetBaudRate(UART_GPS, 4800);
	uartSetRxHandler(UART_GPS, GPSuartRxHandler) ;

#ifdef SIRF_SETTINGS
	// param�trage de la liaison s�rie
	sendGPSCommand("PSRF100,1,4800,8,1,0") ;

	// d�finition des messages de sortie
	// - commence par d�sactiver tous les messages en les configurant en setRate(0)
	char *cmde = "PSRF103,?,0,0,0";
	u08 msgId;
	for (msgId = '0'; msgId < '6'; msgId++) {
		cmde[8] = msgId ;
		sendGPSCommand(cmde) ;
	}
	// - configure le RMC en p�riodique 1 seconde
	cmde[8] = '4'; // '4' = id message RMC
	cmde[12] = '1'; // setRate(1)
	sendGPSCommand(cmde) ;
#else
	// param�trage des output sentences : seulement GPRMC
	sendGPSCommand("PGRMO,,2") ; 		// d�sactive tous les messages
	sendGPSCommand("PGRMO,GPRMC,1") ;	// active le message GPRMC

	/*
	 * On attend suffisamment de temps pour laisser le temps d'arriver au
	 * premier message. Ainsi, on saura d�s le d�but si le GPS est l�.
	 */
	for (u08 i = 15 ; i ; i--) {	// 1.5 secondes d'attente maxi
		delay_10ms(10) ;
		if (rf_system_frame.hw_flags | _BV(HW_GPS_BIT)) break ;
	}
#endif
}

/* *************************************************************************************************
 * GESTION DE LA REPONSE RADEAU
 * ************************************************************************************************* */

/*
 * Pr�paration et envoi de la r�ponse du radeau
 *
 * Afin de r�duire la consommation de l'�metteur, on utilise 2 types de trames :
 * - les trames de retour de donn�es (data packets)
 *   elles contiennent l'ensemble des informations collect�es au niveau du radeau
 *   (mesures, position, �tat des batteries)
 * - les trames de "bonne sant�" (health packet), simple paquet sans contenu
 *
 * Les trames de donn�es ne sont envoy�es qu'avec une p�riodicit� d'une seconde,
 * soit une toutes les 10 trames du poste de contr�le. Les 9 autres trames ne sont
 * que des health packets.
 */

// -------------------------------------------------------------------------------------------------

/*
 * Initialisation du canal retour
 */
static void back_channel_init(void) {
	memset(&rf_extended_frame, 0, sizeof(rf_extended_frame)) ;
	rf_extended_frame.header.frame_start = STX;
	rf_extended_frame.header.frame_tag = TAG_EXTENDED;
	rf_extended_frame.header.payload_len = sizeof(rf_extended_frame) - sizeof(FRAME_HEADER);

	memset(&rf_user_frame, 0, sizeof(rf_user_frame));
	rf_user_frame.header.frame_start = STX;
	rf_user_frame.header.frame_tag = TAG_USER;
	rf_user_frame.header.payload_len = sizeof(rf_user_frame) - sizeof(FRAME_HEADER);

	memset(&rf_position_frame, 0, sizeof(rf_position_frame));
	rf_position_frame.header.frame_start = STX;
	rf_position_frame.header.frame_tag = TAG_POSITION;
	rf_position_frame.header.payload_len = sizeof(rf_position_frame) - sizeof(FRAME_HEADER);

	memset(&rf_system_frame, 0, sizeof(rf_system_frame));
	rf_system_frame.header.frame_start = STX;
	rf_system_frame.header.frame_tag = TAG_SYSTEM;
	rf_system_frame.header.payload_len = sizeof(rf_system_frame) - sizeof(FRAME_HEADER);

	rf_filler_frame.header.frame_start = STX;
	rf_filler_frame.header.frame_tag = TAG_FILLER;
	rf_filler_frame.header.payload_len = sizeof(rf_filler_frame) - sizeof(FRAME_HEADER);
	rf_filler_frame.frame_cnt = '0';

	memset(&rf_msg_frame, 0, sizeof(rf_msg_frame));
	rf_msg_frame.frame_header.frame_start = STX;
	rf_msg_frame.frame_header.frame_tag = TAG_MSG;
}

/*
 * Transmission des donn�es sur le canal retour
 *
 * Le type de r�ponse � retourner (�ventuellement aucune) a �t� indiqu� dans la derni�re
 * trame de commande re�ue.
 */

static void back_channel_transmit(void) {
	switch (expected_reply) {
	case TAG_EXTENDED :
		memcpy((char *)&rf_extended_frame.position,
				(char *)&rf_position_frame.pos_ll,
				sizeof(rf_extended_frame.position)
				) ;
		memcpy((char *)&rf_extended_frame.system,
				(char *)&rf_system_frame.battery_logic,
				sizeof(rf_extended_frame.system)
				) ;
		memcpy((char *)&rf_extended_frame.user,
				(char *)&rf_user_frame.adc_local,
				sizeof(rf_extended_frame.user)
				) ;
		uartSendBuffer(UART_RF, (char *)&rf_extended_frame, sizeof(rf_extended_frame)) ;
		break ;

	case TAG_POSITION :
		uartSendBuffer(UART_RF, (char *)&rf_position_frame, sizeof(rf_position_frame)) ;
		break ;

	case TAG_SYSTEM :
		uartSendBuffer(UART_RF, (char *)&rf_system_frame, sizeof(rf_system_frame)) ;
		break ;

	case TAG_USER :
		uartSendBuffer(UART_RF, (char *)&rf_user_frame, sizeof(rf_user_frame)) ;
		break ;

	case TAG_FILLER :
		rf_filler_frame.frame_cnt++;
		uartSendBuffer(UART_RF, (char *)&rf_filler_frame, sizeof(rf_filler_frame)) ;
		break ;

	case TAG_NONE :
		break ;

	default:
		break ;
	}

	// si des donn�es ont �t� envoy�es en r�ponse, on d�clenche une nouvelle collecte
	if ((expected_reply != TAG_NONE) && (expected_reply != TAG_FILLER)) {
		collect_data_request = TRUE;
	}
}

/*
 * Lecture du cap sur le compas I2C.
 *
 * La valeur est retourn�e en 1/10e de degr�
 */
#define COMPASS_ADDR			0xC0
#define COMPASS_HEADING_REG		2

#define FILTER_HEADING
#ifdef FILTER_HEADING
#define FILTER_SIZE_LOG2	2
#define FILTER_SIZE 		(1 << FILTER_SIZE_LOG2)
#endif

static void read_compass(void) {
	static u08 regAddr= COMPASS_HEADING_REG;
	u16 hdg ;

	i2cMasterSend(COMPASS_ADDR, 1, &regAddr) ;
	i2cMasterReceive(COMPASS_ADDR, sizeof(hdg), (u08 *)&hdg) ;
	hdg = (hdg >> 8) + ((hdg & 0x00ff) << 8) ;		// swap bytes

#ifdef FILTER_HEADING
	// filtrage des variations par moyenne glissante

	static u08 sliding_frame_ok = FALSE ;
	static u08 ndx = 0 ;
	static s16 meas[FILTER_SIZE] ;
#define UNSET 0xffff
	static s16 last_meas = UNSET ;
	static u08 shifted_values = 0 ;		// compteur du nombre de valeurs d�call�es cons�cutives

// ATTENTION : on travaille en 1/10 de degr� ;-)
#define _360DEG		3600
#define _180DEG		1800

	/*
	 * Gestion de la discontinuit� de la valeur du cap (du fait du modulo 360)
	 *
	 * Si l'�cart avec la derni�re valeur lue d�passe 180, on se situe autour de la discontinuit�.
	 * On rend alors la fonction continue en d�callant la valeur lue du modulo, et dans le sens de
	 * la diff�rence.
	 * Pour �viter une d�rive continue (si on tourne toujours dans le m�me sens par exemple), le contenu
	 * du buffer circulaire est normalis� (recall� dans l'intervalle normal) d�s qu'on d�tecte
	 * qu'il ne contient que des valeurs d�call�es.
	 */
#define HANDLE_DISC
#ifdef HANDLE_DISC
	if (last_meas != UNSET) {
		s16 delta = (s16)hdg - last_meas ;
		if (delta > _180DEG) {
			meas[ndx] = (s16)hdg - _360DEG ;
			shifted_values++ ;
		} else if (delta < -_180DEG) {
			meas[ndx] = hdg + _360DEG ;
			shifted_values++ ;
		} else {
			meas[ndx] = (s16)hdg ;
			shifted_values = 0 ;
		}
	} else
#endif
		meas[ndx] = hdg ;

	if (sliding_frame_ok) {
		// si toutes les valeurs du buffer sont en dehors de [0,360[, on les normalise
		if (shifted_values == FILTER_SIZE) {
			for (int i = 0 ; i < FILTER_SIZE ; i++) {
				if (meas[i] < 0) {
					meas[i] += _360DEG ;
				} else if (meas[i] >= _360DEG) {
					meas[i] -= _360DEG ;
				}
			}

			// remise � 0 du dispositif
			shifted_values = 0 ;
		}

		// calcul de la moyenne
		s16 avg = 0 ;
		for (int i = 0 ; i < FILTER_SIZE ; i++) avg += meas[i] ;
		avg /= FILTER_SIZE ;

		// normalisation du r�sultat
		if (avg < 0) {
			rf_position_frame.heading = avg + _360DEG ;
		} else if (avg >= _360DEG) {
			rf_position_frame.heading = avg - _360DEG ;
		} else {
			rf_position_frame.heading = avg ;
		}
	}

	// conservation de la derni�re valeur prise en compte pour calcul de la variatio au prochain coup
	last_meas = meas[ndx] ;

	if (++ndx == FILTER_SIZE) {
		ndx = 0 ;						// rebouclage du buffer circulaire
		sliding_frame_ok = TRUE ;		// le buffer est complet => on peut utiliser ses valeurs
										// (on a rat� une valeur, mais ce n'est pas dramatique)
	}
#else
	rf_position_frame.heading = hdg ;
#endif
}

/*
 * Lecture des ADC locaux
 */

// ADC en entr�e directe sur le MCU
#define ADC_LOCAL_CNT 	LOCAL_SENSORS_CNT
#define ADC_LOCAL_MIN	0
#define ADC_LOCAL_MAX	(ADC_LOCAL_CNT - 1)

static void read_ADC(void) {
	u08 i;
	for (i = 0; i < ADC_LOCAL_CNT; i++) {
		rf_user_frame.adc_local[i] = a2dConvert8bit(i) ;
	}
}

/*
 * Lecture des ADC i2c (PCF8591)
 */

// ADC sur PCF8591
#define ADC_I2C_CNT 	REMOTE_SENSORS_CNT
#define ADC_I2C_MIN		0
#define ADC_I2C_MAX		(ADC_I2C_CNT - 1)

#define REMOTE_SENSORS_ADDR		0x90

static void read_i2c_sensors(void) {
	u08 channel;
#define PCF8591_AUTOINC
#ifndef PCF8591_AUTOINC
	for (channel = 0; channel < ADC_I2C_CNT; channel++) {
		// envoi de la commande de s�lection du canal � lire, avec activation de l'oscillateur interne
		u08 ctlByte = 0x00 | channel ;
		i2cMasterSend(REMOTE_SENSORS_ADDR, 1, &ctlByte) ;

		// lecture de la conversion (le premier byte est le r�sultat de la derni�re conversion)
		u08 data[2];
		i2cMasterReceive(REMOTE_SENSORS_ADDR, 2, data) ;

		rf_user_frame.adc_remote[channel] = data[1];
	}
#else
	u08 ctlByte = 0x44 ;	// autoinc + analog output enable (cf datasheet �7.2 page 5)
	i2cMasterSend(REMOTE_SENSORS_ADDR, 1, &ctlByte) ;

	// lecture des conversions (5 bytes car le premier est le reliquat de la pr�c�dente conversion)
	u08 data[5] ;
	i2cMasterReceive(REMOTE_SENSORS_ADDR, sizeof(data), data) ;

	// transfert des donn�es re�ues (en ignorant le 1er byte)
	for (channel = 0 ; channel < ADC_I2C_CNT; channel++) {
		rf_user_frame.adc_remote[channel] = data[channel + 1];
	}
#endif
}

/*
 * Lecture des IOs syst�me
 */
inline static void read_system_ios(void) {
	rf_system_frame.system_IOs = ((SYSTEM_FLAGS_PORT & SYSTEM_FLAGS_BITS & ~_BV(WINCH_TICK_BIT)) >> 4) & 0x0f ;
}

/*
 * Lecture des niveaux batteries
 */
inline static void read_battery_levels(void) {
	rf_system_frame.battery_logic= a2dConvert8bit(ADC_BATT_LEVEL_LOGIC) ;
	rf_system_frame.battery_power= a2dConvert8bit(ADC_BATT_LEVEL_POWER) ;
}

/*
 * Collecte des donn�es renvoy�es par le radeau
 *
 * Afin de ne pas trop espacer les autres traitements, la collecte des donn�es
 * est fractionn�e en plusieurs �tapes. Une seule de ces �tapes est ex�cut�e � chaque
 * appel, la derni�re remettant � z�ro la s�quence et la requ�te de collecte.
 */
static void collect_data(void) {
	static u08 collect_data_step = 0;

	switch (collect_data_step) {
	case 0:
		// lit le compas (si pr�sent)
		if (compass_present)
			read_compass() ;
		collect_data_step++;
		break;

	case 1:
		// lit les entr�es analogiques
		read_ADC() ;
		collect_data_step++;
		break;

	case 2:
		// lit les capteurs I2C (si pr�sents)
		if (remote_sensors_present)
			read_i2c_sensors() ;
		collect_data_step++;
		break;

	case 3:
		// lit les IOs syst�me
		read_system_ios() ;
		collect_data_step++;
		break;

	case 4:
		// lit les tensions batteries
		read_battery_levels() ;
		collect_data_step++;
		break;

	case 5:
		if (gps_data_available) {
			gps_process_GPRMC(gps_buffer, &rf_position_frame.pos_ll) ;
		}
		collect_data_step++;
		break;

	default:
		// remise � z�ro du processus de collecte
		collect_data_request = FALSE; // la collecte en cours est termin�e
		collect_data_step = 0; // on repart de la premi�re �tape
	}
}

/* *************************************************************************************************
 * LIAISON RF POSTE DE CONTROLE / RADEAU
 * ************************************************************************************************* */

// taille du buffer de r�ception RF (2 * la taille du bloc de commande, par s�curit�)
#define RF_RX_BUFFER_SIZE (2 * sizeof(rc_cmdblock))

// buffer de r�ception UART liaison RF
u08 rf_rx_bytes[RF_RX_BUFFER_SIZE];

char *rf_buffer_insert = rf_rx_bytes ;
u08 rf_data_len = 0;

/*
 * Interrupt handler de l'UART assign� au modem RF
 */
static void RFuartRxHandler(unsigned char c) {
	switch (c) {
	case STX:
		// d�but d'une trame
		rf_buffer_insert = rf_rx_bytes ;
		rf_data_len = 0;
		cmd_state = CMD_RECEIVING;
		rx_frame_cnt++ ;
		break;

	case ETX:
		// si on n'a pas termin� le traitement de la commande pr�c�dente, on
		// laisse tomber celle-ci
		if (cmd_state != CMD_RECEIVING) {
			cmd_state = CMD_IDLE ;
			return;
		}

		// copie du buffer de r�ception afin de le lib�rer vers la zone d'analyse de la commande
		memcpy(&rc_cmdblock, rf_rx_bytes, sizeof(rc_cmdblock));
		cmd_state = CMD_RECEIVED;
		break;

	default:
		// ajout du caract�re en fin de buffer de r�ception
		if (cmd_state == CMD_RECEIVING) {
			if (rf_data_len < RF_RX_BUFFER_SIZE) {
				*rf_buffer_insert++ = c;
				rf_data_len++;
			}
		}
	}
}

/*
 * Initialisation
 */

static void setupRFLink(void) {
	uartSetBaudRate(UART_RF, 19200) ;
	uartSetRxHandler(UART_RF, RFuartRxHandler) ;

	// passage en mode Hayes
	uartSendBuffer(UART_RF, "+++", 3) ;
	delay_10ms(5) ;

	/*
	 * Configure le modem en fonction de la puissance s�lectionn�e :
	 * - jumper en place : 500mW
	 * - jumper retir� : 100mW
	 */
	static char* ATS202 = "ATS202=?\r";
#define ATS202_PARM (*(ATS202 + 7))

	ATS202_PARM = (bit_is_clear(RF_POWER_SEL_PORT, RF_POWER_SEL_BIT)) ? '2' : '1';
	uartSendBuffer(UART_RF, ATS202, 9) ;
	delay_10ms(5) ;

	// sortie du mode Hayes => passage en mode communication
	uartSendBuffer(UART_RF, "AT0\r", 4) ;
	delay_10ms(50) ;
}

/* *************************************************************************************************
 * ************************************************************************************************* */

int main(void) {
	init_devices();

	uartInit() ;

	back_channel_init() ;
	setupRFLink();
	sendTextMessage("MEDUSA 1 : initalisation syst�me") ;

	i2cInit();

	// configuration de l'ADC
	a2dInit() ;
	a2dSetPrescaler(ADC_PRESCALE_DIV32);
	// AREF �tant connect� � une source de tension sur l'AVR-STAMP,
	// on est oblig� de l'utiliser comme r�f�rence
	// (cf datasheet ATMega128 p238 pour l'explication)
	a2dSetReference(ADC_REFERENCE_AREF);

	setupGPS() ;

	// d�termine quels sont les �quipements pr�sents en v�rifiant qu'ils r�pondent

	// - compas
	if (i2cMasterSendNI(COMPASS_ADDR, 0, 0) == I2C_OK) {
		rf_system_frame.hw_flags |= _BV(HW_COMPASS_BIT) ;
	}

	// - capteurs I2C remote
	if (i2cMasterSendNI(REMOTE_SENSORS_ADDR, 0, 0)== I2C_OK) {
		rf_system_frame.hw_flags |= _BV(HW_REMOTE_SENS_BIT) ;
	}

	// - sondes de temp�rature moteur (1wire)
	// active le processus de lecture p�riodique des temp�ratures
	select_mtemp_1() ;
	DS18X20_start_meas(TRUE, NULL) ;
	select_mtemp_2() ;
	DS18X20_start_meas(TRUE, NULL) ;
	ds18x20_state = DSSTAT_START_CONV ;

	/*
	while(ds18x20_state != DSSTAT_READ_TEMP) { }

	u08 sub_zero, cel_frac ;
	select_mtemp_1() ;
	// on se contente des degr�s et on est certain d'�tre au-dessus de 0 :-)
	if (DS18X20_read_meas_single(DS18S20_ID, &sub_zero, &rf_system_frame.temp_motor[0], &cel_frac) == DS18X20_OK) {
		rf_system_frame.hw_flags |= _BV(HW_MOT1_TEMP_BIT) ;
	}

	select_mtemp_2() ;
	if (DS18X20_read_meas_single(DS18S20_ID, &sub_zero, &rf_system_frame.temp_motor[1], &cel_frac) == DS18X20_OK) {
		rf_system_frame.hw_flags |= _BV(HW_MOT2_TEMP_BIT) ;
	}

	ds18x20_state = DSSTAT_READ_TEMP_DONE ;
	*/

	sendTextMessage("Initialisation ok") ;

#define VERBOSE 1
#ifdef VERBOSE
	sendTextMessage("Equipements :") ;

	static char *msg_found = "OK";
	static char *msg_not_found = "--";
	static char msg[MSG_LEN_MAX];

	strcpy(msg, "- GPS ................ ") ;
	strcat(msg, gps_present ? msg_found : msg_not_found) ;
	sendTextMessage(msg) ;

	strcpy(msg, "- compas ............. ") ;
	strcat(msg, compass_present ? msg_found : msg_not_found) ;
	sendTextMessage(msg) ;

	strcpy(msg, "- sonde immerg�e ..... ") ;
	strcat(msg, remote_sensors_present ? msg_found : msg_not_found) ;
	sendTextMessage(msg) ;

	/*
	 * le statut des sondes temp�rature moteur sera d�couvert par la suite,
	 * lors des demandes de lecture
	 */
#endif

	/*
	 * Active les ESC des moteurs de propulsion. Celui du treuil est
	 * g�r� s�par�ment pour prendre en compte les s�curit�s de fin de course
	 */
	servo_enable(ESC_MOTOR_1_NUM) ;
	servo_enable(ESC_MOTOR_2_NUM) ;

	// la boucle principale

	while (1) {
		if (cmd_state == CMD_RECEIVED) {
			process_rcblock() ;
			back_channel_transmit() ;
		}

		if (collect_data_request) {
			collect_data() ;
		}

		winch_control() ;

		/*
		 * lecture des DS18x20 (cadenc�e au niveau du timer 1 seconde)
		 */
		switch (ds18x20_state) {
		case DSSTAT_START_CONV:
			select_mtemp_1() ;
			if (DS18X20_start_meas(TRUE, NULL) == DS18X20_OK) {
				ds18x20_state = DSSTAT_START_CONV_DONE ;
			}
			select_mtemp_2() ;
			if (DS18X20_start_meas(TRUE, NULL) == DS18X20_OK) {
				ds18x20_state = DSSTAT_START_CONV_DONE ;
			}
			break ;

		case DSSTAT_READ_TEMP: {
				u08 sub_zero, cel_frac ;
				select_mtemp_1() ;
				// on se contente des degr�s et on est certain d'�tre au-dessus de 0 :-)
				if (DS18X20_read_meas_single(DS18S20_ID, &sub_zero, &rf_system_frame.temp_motor[0], &cel_frac) == DS18X20_OK) {
					rf_system_frame.hw_flags |= _BV(HW_MOT1_TEMP_BIT) ;
				}

				select_mtemp_2() ;
				if (DS18X20_read_meas_single(DS18S20_ID, &sub_zero, &rf_system_frame.temp_motor[1], &cel_frac) == DS18X20_OK) {
					rf_system_frame.hw_flags |= _BV(HW_MOT2_TEMP_BIT) ;
				}

				ds18x20_state = DSSTAT_READ_TEMP_DONE ;
			}
			break ;
		}

		/*
		 * arr�t des moteurs et coupure des relais si pas de r�ception de trame depuis trop longtemps
		 */
		if (communication_lost) {
			motor_disable_all();
			set_winch_command(WINCH_CMD_DISABLE, 0);
			all_relays_off() ;

		} else {
			// pour retour de la communication
			motor_enable_all();
		}
	} // while (1)
}

