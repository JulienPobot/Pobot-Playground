#include <LiquidCrystal.h>

/**
 * Test avec la librairie "Liquid Crystal"
 */
 
 LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
 
 void setup()
 {
   // on définit les 16 colonnes et 2 lignes
   lcd.begin(16,2);
   
   // on affiche un premier message
   lcd.print("test LCD Pobot");
   
 }
 
 
void loop() {
  // on passe à la ligne 2 (on commence à compter à 1)
  lcd.setCursor(0, 1);
  // et on affiche le nombre de dixièmes de secondes
  lcd.print(millis()/100);
}


