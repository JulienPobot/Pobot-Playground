/*
    Programme de chenillard bourrin, illustrant tout juste l'utilisation des ports d'I/O.

    Cible : ATMega16
    Quartz : 8MHz
    Compilateur : avr-gcc (WInAVR)
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>

/*
    Initialisation des ports
*/ 
void port_init(void) {
    // configuration du port D
     // - toutes les I/O en sortie
     DDRD  = 0xFF ;
    // - toutes les LEDs �teintes
     PORTD = 0x00 ;
}

/*
    Initialisation des p�riph�riques
*/
void init_devices(void) {
     cli();                         // inhibition des interruptions (pour avoir la paix)
     port_init();                // initialisation de prts
    
    // MCU Control Register
    // - interruptions externes sur niveau bas de la ligne
    // - sleep mode d�sactiv�
     MCUCR = 0x00;            
     
     // General Interrupt Control
     // - INT0 et INT1 inactives
     GICR  = 0x00;
     
     // Timer/Counter Interrupt Mask
     // - aucune interruption timer activ�e
     TIMSK = 0x00;     
     
     sei();                           // autorisation des interruptions
}

/* 
    G�n�ration d'un d�lai suffisant pour le d�filement
*/
void Delay(void) {
    unsigned char a, b;
    int i ;

    // � la m�thode bourrin : on boucle le nombre de fois qui va bien
    for (i=0;i<5;i++) 
        for (a = 1; a; a++)
            for (b = 1; b; b++)
                ;
}

/*
    Affichage d'une LED
    
    Param�tres:
    i : num�ro de la LED (0..7)
*/
void LED_On(int i)    {
    // on met � 1 le bit correspondant � la sortie � passer � l"�tat haut
    PORTD = (1 << i) ;
    // et on attend un peu
    Delay();
}

//- --------------------------------------------------------------------------------
int main(void) {
    int i;
    
    init_devices() ;

    while (1) {
        // d�filement avant
        for (i = 0; i < 8; i++)
            LED_On(i);
            
        // d�filement arri�re
        for (i = 8; i > 0; i--)
            LED_On(i);
            
        // une sur deux en avant,
        for (i = 0; i < 8; i += 2)
            LED_On(i);
        // puis en arri�re
        for (i = 7; i > 0; i -= 2)
            LED_On(i);
            
        // et un flash g�n�ral pour finir
        for (i=0; i<4; i++) {
            PORTD = 0xff ;
            Delay() ;
            PORTD = 0 ;
            Delay() ;
        }
    }
}
