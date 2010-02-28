/*
    Bibliothèque de fonctions pour l'utilisation de l'USART

    Documentation API : voir usart.h
    
    Cible : ATMega8
    Quartz : 8MHz
    Compilateur : avr-gcc (WInAVR)
*/

#include <avr/io.h>
#include <avr/interrupt.h>
//#include <avr/signal.h>

#include "usart.h"

// descripteur de buffer circulaire pour les communications série
typedef struct {
    volatile unsigned char head ;            // pointeur d'écriture
    volatile unsigned char tail ;            // pointeur de lecture
    volatile unsigned char nbchars ;        // nombre de caractères en attente
    volatile unsigned char size ;            // taille du buffer
} BUFFER_DESC ;

// buffer de réception
static char rxbuff[32] ;
static volatile BUFFER_DESC rxdesc ;

// buffer d'émission
static char txbuff[32] ;
static volatile BUFFER_DESC txdesc ;

/*
    Interrupt handler USART Rx complete

    Déclenchée dès qu'un caractère est disponible dans le registre de réception
*/
SIGNAL (SIG_UART_RECV) 
{
    // lecture du caractère reçu
    char c = UDR ;

    // si buffer pas plein, on le stocke
    if (rxdesc.nbchars < rxdesc.size) 
	{
        // stockage du caractère
        rxbuff[rxdesc.head] = c ;
        // avance du pointeur de tête avec gestion du rebouclage
        if (++rxdesc.head == rxdesc.size)
			rxdesc.head = 0 ;
        // actualisation du nombre de caractères en attente
        rxdesc.nbchars++ ;
    }
}

/*
    Interrupt handler USART Data Register Empty

    Déclenchée tant que le registre de transmission est vide
*/
SIGNAL (SIG_UART_DATA) 
{
    char c ;

    // extrait le caractère à envoyer depuis le buffer
    c = txbuff[txdesc.tail] ;
    // met à jour le nombre de caractères en attente dans le buffer
    --txdesc.nbchars ;
    // s'il n'y en a plus d'autre, on désactive l'interruption
    if (txdesc.nbchars == 0) 
		UCSRB &= ~(1 << UDRIE) ;
    // met à jour le pointeur d'extraction, en gérant le rebouclage
    if (++txdesc.tail == txdesc.size) 
		txdesc.tail = 0 ;
    // et envoie le caractère
    UDR = c ;
}

/*
      Initialisation de l'USART
*/
void usart_init(int ubrr) 
{
    UBRRH = (unsigned char)(ubrr >> 8) ;
    UBRRL = (unsigned char)ubrr ;

    // format de frame : 8bits, pas de parité, 1 stop bit
    UCSRC = (1 << URSEL)    // sélection de UCSRC (cf DS p 149 & 153)
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
    Lecture d'un caractère sur l'USART
*/
unsigned char usart_getc(void) 
{
    unsigned char c ;

    // attente de disponibilité d'un caractère
    while (rxdesc.nbchars == 0) ;

    // masquage des interruptions pour éviter une altération du buffer pendant
    // le traitement
    cli() ;

    // extraction du caractère
    c = rxbuff[rxdesc.tail] ;
    // mise à jour du pointeur d'extraction, avec rebouclage éventuel
    if (++rxdesc.tail == rxdesc.size) 
		rxdesc.tail = 0 ;
    // ... et du nombre de caractères en attente
    rxdesc.nbchars-- ;

    // rétablissement des interruptions
    sei() ;

    return c ;
}

/*
    Envoi d'un caractère sur l'USART
*/
void usart_putc(unsigned char c) 
{
    // si le buffer de transmission est plein, on attend qu'il y ait de la place
    while (txdesc.nbchars == txdesc.size) ;

    // masquage des interruptions pendant l'accès au buffer, pour être certain
    // que personne d'autre ne va le modifier
    cli() ;

    // ajout du caractère
    txbuff[txdesc.head] = c ;
    // mise à jour du pointeur d'injection, avec rebouclage éventuel
    if (++txdesc.head == txdesc.size) 
		txdesc.head = 0 ;

    // si c'est le premier caractère dans le buffer, on active l'interruption
    // Data Register Empty de l'USART pour commencer à "pomper" le buffer via
    // le handler de l'interruption
    if (txdesc.nbchars == 0) 
		UCSRB |= (1 << UDRIE) ;

    // mise à jour du nombre de caractères en attente
    txdesc.nbchars++ ;

    // autorisation des interruptions pour continuer le travail
    sei() ;
}

/*
    Envoi d'une chaîne de caractères sur l'USART
*/
void usart_puts(char *s) 
{
    unsigned char c ;

    while ((c = *s++) != 0) 
	{
        usart_putc(c) ;
    }
}
