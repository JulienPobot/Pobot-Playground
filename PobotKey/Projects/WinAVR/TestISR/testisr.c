/**
 * Programme de démonstration d'utilisation d'interruptions avec AVR-GCC
 *
 * - utilisation d'un ATtiny13
 * - utilisation d'une interruption externe
 *
 * Ce programme fait clignoter en permanence la LED reliée à la pin PB0 du MCU
 * et change l'état de celle qui est reliée à la pin PB2 lorsqu'on appuie sur
 * le poussoir connecté sur la pin PB1 (qui correspond au signal INT0).
 */

#include "avr/io.h"
#include "avr/interrupt.h"
#include "util/delay.h"

/** Déclaration des fonctions **/

static void avr_init(void);

/** Gestion de l'interruption **/

/*
 * Cette interruption est déclenchée dès que l'état de la pin PB1 passe
 * de 1 à 0, ce qui se produit lorsque le poussoir est appuyé.
 * Ce comportement est défini par les initialisations effectuées en début de
 * programme (voir les commentaires plus loin)
 */

ISR(INT0_vect)
{
 // on change l'état de la LED en testant son état actuel et en
 // le modifiant en conséquence
       if (bit_is_set(PORTB,2)) {
               PORTB &= ~(_BV(2));     
       } else {
               PORTB |= _BV(2);                
       }

/* à noter que sauf erreur ça peut se faire plus simplement
  avec un XOR comme ceci :
 PORTB ^= _BV(2) ;
*/
}

/**
 *
 * MAIN
 *
 */
int main(void)
{
   // configuration du MCU
   avr_init();

   // Cette boucle sans fin fait clignoter la LED PB0 en permanence
   for(;;)
   {
               PORTB |= _BV(0);                
               _delay_ms(500);         
               PORTB &= ~(_BV(0));             
               _delay_ms(250);
                       
   }

   // ce return n'est pas vraiment utile, puisqu'on n'y arrivera jamais        
   return(0);
}

static void avr_init(void)
{
       // on inhibe les interruptions le temps de mettre en place la
       // configuration (pas obligatoire, mais sage précaution pour
       // éviter les départs en vrille lorsque la mise en place d'une
       // configuration est susceptible de déclencher une interruption
       // alors qu'on n'y est pas encore préparé)
       cli();  

   // configuration des entrées-sorties
       
       DDRB |= _BV(0); // pin PB0 en sortie
       DDRB |= _BV(2); // pin PB2 en sortie
       
       DDRB &= ~(_BV(1)); // pin PB1 en entrée
       
       // configuration de l'interruption utilisée

       // Activer l'interruption externe "INT0"
       GIMSK |= _BV(INT0);

       // Définir un déclenchement sur front descendant
       // - lire la datasheet
       // - trouver le tableau qui dit que ISC00 = 0 et ISC01 = 1 pour le      trigger falling-edge
       
       MCUCR |= _BV(ISC01);

       // on autorise les interruptions à partir de maintanant
       sei();  

   return;
}
    
	
    