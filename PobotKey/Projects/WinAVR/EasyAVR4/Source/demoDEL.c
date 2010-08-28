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
    // - toutes les LEDs éteintes
     PORTD = 0x00 ;
}

/*
    Initialisation des périphériques
*/
void init_devices(void) {
     cli();                         // inhibition des interruptions (pour avoir la paix)
     port_init();                // initialisation de prts
    
    // MCU Control Register
    // - interruptions externes sur niveau bas de la ligne
    // - sleep mode désactivé
     MCUCR = 0x00;            
     
     // General Interrupt Control
     // - INT0 et INT1 inactives
     GICR  = 0x00;
     
     // Timer/Counter Interrupt Mask
     // - aucune interruption timer activée
     TIMSK = 0x00;     
     
     sei();                           // autorisation des interruptions
}

/* 
    Génération d'un délai suffisant pour le défilement
*/
void Delay(void) {
    unsigned char a, b;
    int i ;

    // à la méthode bourrin : on boucle le nombre de fois qui va bien
    for (i=0;i<5;i++) 
        for (a = 1; a; a++)
            for (b = 1; b; b++)
                ;
}

/*
    Affichage d'une LED
    
    Paramètres:
    i : numéro de la LED (0..7)
*/
void LED_On(int i)    {
    // on met à 1 le bit correspondant à la sortie à passer à l"état haut
    PORTD = (1 << i) ;
    // et on attend un peu
    Delay();
}

//- --------------------------------------------------------------------------------
int main(void) {
    int i;
    
    init_devices() ;

    while (1) {
        // défilement avant
        for (i = 0; i < 8; i++)
            LED_On(i);
            
        // défilement arrière
        for (i = 8; i > 0; i--)
            LED_On(i);
            
        // une sur deux en avant,
        for (i = 0; i < 8; i += 2)
            LED_On(i);
        // puis en arrière
        for (i = 7; i > 0; i -= 2)
            LED_On(i);
            
        // et un flash général pour finir
        for (i=0; i<4; i++) {
            PORTD = 0xff ;
            Delay() ;
            PORTD = 0 ;
            Delay() ;
        }
    }
}
