/*
    USART routines
    
    Cible : ATMega8
    Quartz : 8MHz
    Compilateur : avr-gcc (WinAVR)
*/

#ifndef _USART_H_
#define _USART_H_ 1

/* 
    Valeur UBRR pour diff�rentes vitesses et fr�quences de quartz.
    
    Codification : BAUD_<Fquartz>_<baud>
    
    Note: seules les valeurs donnant une erreur acceptable (ie. <= 0.5%) sont d�finies
*/
    
#define UBRR_8_2400        207
#define UBRR_8_4800        103
#define UBRR_8_9600        51
#define UBRR_8_19200    25
#define UBRR_8_28800    12

/*
    Initialise l'USART
    
    Param�tres:
    ubrr : valeur � donner � UBRR poour le couple (vitesse, Fquartz)
           (cf Datasheet p156 et suivantes)
*/
void usart_init(int ubrr) ;

/*
    Attend le prochain caract�re en r�ception et le retourne
    
    R�sultat : caract�re arriv�
*/
unsigned char usart_getc(void) ;

/*
    Envoie un caract�re sur l'USART
    
    Param�tres:
    c : caract�re � envoyer
*/
void usart_putc(unsigned char c) ;

/*
    Envoie une cha�ne de caract�res sur l'USART
    
    Param�tres:
    s : cha�ne de caract�res � envoyer
*/
void usart_puts(char *s) ;

char is_char_available(void);
#endif
