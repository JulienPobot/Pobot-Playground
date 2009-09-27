/*
    Biblioth�que de fonctions pour l'utilisation de l'USART

    Documentation API : voir usart.h
    
    Cible : ATMega8
    Quartz : 8MHz
    Compilateur : avr-gcc (WInAVR)
*/

#include <avr/io.h>
#include <avr/interrupt.h>
//#include <avr/signal.h>

#include "usart.h"

// descripteur de buffer circulaire pour les communications s�rie
typedef struct {
    volatile unsigned char head ;            // pointeur d'�criture
    volatile unsigned char tail ;            // pointeur de lecture
    volatile unsigned char nbchars ;        // nombre de caract�res en attente
    volatile unsigned char size ;            // taille du buffer
} BUFFER_DESC ;

// buffer de r�ception
static char rxbuff[32] ;
static volatile BUFFER_DESC rxdesc ;

// buffer d'�mission
static char txbuff[32] ;
static volatile BUFFER_DESC txdesc ;

/*
    Interrupt handler USART Rx complete

    D�clench�e d�s qu'un caract�re est disponible dans le registre de r�ception
*/
SIGNAL (SIG_UART_RECV) 
{
    // lecture du caract�re re�u
    char c = UDR ;

    // si buffer pas plein, on le stocke
    if (rxdesc.nbchars < rxdesc.size) 
	{
        // stockage du caract�re
        rxbuff[rxdesc.head] = c ;
        // avance du pointeur de t�te avec gestion du rebouclage
        if (++rxdesc.head == rxdesc.size)
			rxdesc.head = 0 ;
        // actualisation du nombre de caract�res en attente
        rxdesc.nbchars++ ;
    }
}

/*
    Interrupt handler USART Data Register Empty

    D�clench�e tant que le registre de transmission est vide
*/
SIGNAL (SIG_UART_DATA) 
{
    char c ;

    // extrait le caract�re � envoyer depuis le buffer
    c = txbuff[txdesc.tail] ;
    // met � jour le nombre de caract�res en attente dans le buffer
    --txdesc.nbchars ;
    // s'il n'y en a plus d'autre, on d�sactive l'interruption
    if (txdesc.nbchars == 0) 
		UCSRB &= ~(1 << UDRIE) ;
    // met � jour le pointeur d'extraction, en g�rant le rebouclage
    if (++txdesc.tail == txdesc.size) 
		txdesc.tail = 0 ;
    // et envoie le caract�re
    UDR = c ;
}

/*
      Initialisation de l'USART
*/
void usart_init(int ubrr) 
{
    UBRRH = (unsigned char)(ubrr >> 8) ;
    UBRRL = (unsigned char)ubrr ;

    // format de frame : 8bits, pas de parit�, 1 stop bit
    UCSRC = (1 << URSEL)    // s�lection de UCSRC (cf DS p 149 & 153)
          | (1 << UCSZ1) | (1 << UCSZ0)         // 8-bits
          ;

    // enable Rx et Tx
    UCSRB = (1 << RXEN)            // enable receive
          | (1 << TXEN)         // enable transmit
          | (1 << RXCIE)        // enable Receive Complete interrupt
          ;

    rxdesc.head = 0 ;
    rxdesc.tail = 0 ;
    rxdesc.nbchars = 0 ;
    rxdesc.size = sizeof(rxbuff) ;

    txdesc.head = 0 ;
    txdesc.tail = 0 ;
    txdesc.nbchars = 0 ;
    txdesc.size = sizeof(txbuff) ;
}

char is_char_available(void)
{
	if(rxdesc.nbchars == 0)
		return 0;
	return 1;
}

/*
    Lecture d'un caract�re sur l'USART
*/
unsigned char usart_getc(void) 
{
    unsigned char c ;

    // attente de disponibilit� d'un caract�re
    while (rxdesc.nbchars == 0) ;

    // masquage des interruptions pour �viter une alt�ration du buffer pendant
    // le traitement
    cli() ;

    // extraction du caract�re
    c = rxbuff[rxdesc.tail] ;
    // mise � jour du pointeur d'extraction, avec rebouclage �ventuel
    if (++rxdesc.tail == rxdesc.size) 
		rxdesc.tail = 0 ;
    // ... et du nombre de caract�res en attente
    rxdesc.nbchars-- ;

    // r�tablissement des interruptions
    sei() ;

    return c ;
}

/*
    Envoi d'un caract�re sur l'USART
*/
void usart_putc(unsigned char c) 
{
    // si le buffer de transmission est plein, on attend qu'il y ait de la place
    while (txdesc.nbchars == txdesc.size) ;

    // masquage des interruptions pendant l'acc�s au buffer, pour �tre certain
    // que personne d'autre ne va le modifier
    cli() ;

    // ajout du caract�re
    txbuff[txdesc.head] = c ;
    // mise � jour du pointeur d'injection, avec rebouclage �ventuel
    if (++txdesc.head == txdesc.size) 
		txdesc.head = 0 ;

    // si c'est le premier caract�re dans le buffer, on active l'interruption
    // Data Register Empty de l'USART pour commencer � "pomper" le buffer via
    // le handler de l'interruption
    if (txdesc.nbchars == 0) 
		UCSRB |= (1 << UDRIE) ;

    // mise � jour du nombre de caract�res en attente
    txdesc.nbchars++ ;

    // autorisation des interruptions pour continuer le travail
    sei() ;
}

/*
    Envoi d'une cha�ne de caract�res sur l'USART
*/
void usart_puts(char *s) 
{
    unsigned char c ;

    while ((c = *s++) != 0) 
	{
        usart_putc(c) ;
    }
}
