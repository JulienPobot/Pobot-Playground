class MobileTest 
{
  /* algorithm variables */

  // position (pixels)
  float posX = 20;

  // where to go
  float aimX;

  // current speed (pixels per second)
  float speed;

  // setting speed (the one that we can control)
  float consigne;
  
  // coeffs
  float Kp;
  float Ki;
  float Kd;

  /* display variables */

  int swimline;

  int baseY;

  boolean over = false;  // If mouse over
  boolean move = false;  // moving

  MobileTest(int lineNumber, float x, float p, float i, float d)
  {
    this.swimline = lineNumber;
    this.aimX = x;
    this.baseY = swimline*100+50;
    this.Kp = p;
    this.Ki = i;
    this.Kd = d;

  }


  void updateMobile()
  {   
    // Update the spring position
    if(!move) {

      // calcul de l'erreur
      float erreur = aimX - posX;

      speed = Kp*erreur;

      if (consigne > 0 && speed < consigne) {
        speed++;  
      }
      if (consigne > 0 && speed > consigne) {
        speed--;  
      }
      if (consigne < 0 && speed < consigne) {
        speed++;  
      }
      if (consigne < 0 && speed > consigne) {
        speed--;  
      }

      posX = posX + speed;

      /*
    f = -K * (ps - R);    // f=-ky
       as = f / M;           // Set the acceleration, f=ma == a=f/m
       vs = D * (vs + as);   // Set the velocity
       ps = ps + vs;         // Updated position
       */
    }
    if(abs(speed) < 0.1) {
      speed = 0.0;
    }

    testOver();

    // Set and constrain the position of top bar
    if(move) {
      posX = mouseX;
      if (posX<10) posX=10;
      if (posX>width-10) posX=width-10;
    }
  } 

  void drawMobile()
  {
    // Draw base (line)
    fill(0.2);
    rect(posX, baseY-4, aimX, baseY+4);

    // Set color and draw mobile (bar)
    if(over || move) { 
      fill(255);
    } 
    else { 
      fill(204);
    }
    rect(posX-8,baseY-40,posX+8,baseY+40);

  }  

  void testOver() {
    // Test if mouse is over the top bar
    if(mouseY > baseY-40 && mouseY < baseY+40 && mouseX > posX-8 && mouseX < posX+8) {
      over = true;
    } 
    else {
      over = false;
    }

  }

  void testMove() {
    if(over) {
      move = true;
    }
  }

  void endMove() {
    move = false; 
  }

}
