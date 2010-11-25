#include <LiquidCrystal.h>

/**
 * Test de l'accéléromètre 3 axes MMA7260Q
 * de Freescales (carte DFRobot)
 */

// le LCD 16x2 (DFRobot aussi)
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// le numéro de la ligne courante
int ligne = 0;

int moyX = 0;
int moyY = 0;
int moyZ = 0;

int valX;
int valY;
int valZ;

void setup()
{
  // on définit les 16 colonnes et 2 lignes
  lcd.begin(16,2);

  // on affiche un premier message
  lcd.print("Test accelero");

  //
  for (int i=0; i<2000; i++) {
   moyX += analogRead(0);
   moyY += analogRead(1); 
   moyZ += analogRead(2);
   delay(5);
  }
  
  moyX = moyX / 2000;
  moyY = moyY / 2000;
  moyZ = moyZ / 2000;

}


void loop() {
  // on passe à la ligne 2 (on commence à compter à 1)
  lcd.setCursor(0, 1);
  // et on affiche le nombre de dixièmes de secondes
  lcd.print(analogRead(0)-moyX);
  lcd.print(" ");
  lcd.print(analogRead(1)-moyY);
  lcd.print(" ");
  lcd.print(analogRead(2)-moyZ);
  lcd.print(" .");
  
}



