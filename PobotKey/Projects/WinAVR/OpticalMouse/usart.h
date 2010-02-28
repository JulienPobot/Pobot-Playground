/*
    USART routines
    
    Cible : ATMega8
    Quartz : 8MHz
    Compilateur : avr-gcc (WinAVR)
*/

#ifndef _USART_H_
#define _USART_H_ 1

/* 
    Valeur UBRR pour différentes vitesses et fréquences de quartz.
    
    Codification : BAUD_<Fquartz>_<baud>
    
    Note: seules les valeurs donnant une erreur acceptable (ie. <= 0.5%) sont définies
*/
    
#define UBRR_8_2400        207
#define UBRR_8_4800        103
#define UBRR_8_9600        51
#define UBRR_8_19200    25
#define UBRR_8_28800    12

/*
    Initialise l'USART
    
    Paramètres:
    ubrr : valeur à donner à UBRR poour le couple (vitesse, Fquartz)
           (cf Datasheet p156 et suivantes)
*/
void usart_init(int ubrr) ;

/*
    Attend le prochain caractère en réception et le retourne
    
    Résultat : caractère arrivé
*/
unsigned char usart_getc(void) ;

/*
    Envoie un caractère sur l'USART
    
    Paramètres:
    c : caractère à envoyer
*/
void usart_putc(unsigned char c) ;

/*
    Envoie une chaîne de caractères sur l'USART
    
    Paramètres:
    s : chaîne de caractères à envoyer
*/
void usart_puts(char *s) ;

char is_char_available(void);
#endif
