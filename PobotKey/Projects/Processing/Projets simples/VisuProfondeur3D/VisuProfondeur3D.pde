/**
 * Visualisation de la profondeur en 3D
 * - en entrée, une image en niveaux de gris
 * - première transformation : passage en couleurs
 * - seconde transformation : mouvements de caméra
 *
 * Code écrit par Julien sur une image 3D de Mathieu
 * (c) Pobot 2010
 *
 * Le code ci-dessous est totalement libre de réutilisation
 * surtout que c'est hyper lent et pas documenté pour le moment.
 *
 */

PImage a; // l'image source
PImage b; // l'image colorée

PImage c; // l'image réelle

String mode = "source";

float eyeX;
float eyeY;
float eyeZ;

boolean reel = false;

void setup()
{
  // charger l'image
  a = loadImage("image_depart2.jpg");
  b = loadImage("image_depart2.jpg");
  c = loadImage("image_reel.jpg");

  // donner sa taille à la fenêtre
  size(a.width, a.height, P3D);

  // paramètres de la fenêtre  
  noStroke();
  background(255);
  smooth();

  // la caméra par défaut
  eyeX = width/2.0;
  eyeY = height/2.0;
  eyeZ = 600.0;

  // analyser l'image  
  a.loadPixels();

  // remplacer les pixels avec la couleur
  for (int i = 0; i < (a.width * a.height); i++)
  {
    colorMode(RGB);
    color c = a.pixels[i];
    float grey = red(c);
    colorMode(HSB);
    // utiliser le niveau de gris
    if (grey > 10) {
      float teinte = map(grey,0,255,255,0);
      color n = color(teinte,255,255);
      b.pixels[i] = n;
    }
  }

  //
  b.updatePixels();
  b.save("image_couleurs.jpg");
}

void keyPressed()
{
  if (key == 'c') {
    mode = "couleurs";
    reel = false;
  } 
  else if (key == 's') {
    mode = "source";
    reel = false;
  }
  else if (key == 'x') {
    mode = "3D";
  }
  else if (key == 'r') {
    mode = "reel";
    reel = true;
  }

  if (key == CODED) {  
    if (keyCode == UP) {
      eyeY -= 100;
      mode = "3D";
    }
    if (keyCode == DOWN) {
      eyeY += 100;
      mode = "3D";
    }
    if (keyCode == LEFT) {
      eyeX -= 100;
      mode = "3D";
    }
    if (keyCode == RIGHT) {
      eyeX += 100;
      mode = "3D";
    }
  }

  if (key == '+') {

    eyeZ += 100;
    mode = "3D";
  } 
  if (key == '-') {

    eyeZ -= 100;
    mode = "3D";
  }
}

void draw()
{
  if (mode.equals("source")) 
  {    
    background(0);
    image(a, 0, 0);
  } 
  if (mode.equals("reel")) 
  {    
    background(0);
    image(c, 0, 0);
  } 
  else if (mode.equals("couleurs"))
  {
    background(0);
    image(b, 0, 0);
  }
  else if (mode.equals("3D"))
  {
    background(0);
    camera(eyeX, eyeY, eyeZ, width/2.0, height/2.0, 128.0, 
    0.0, 1.0, 0.0);
    println("3D en cours");
    sphereDetail(3);
    // parcourir les pixels et les transformer en points dans l'espace
    for (int i = 0; i < b.height; i++)
    {
      print(".");
      // i représente la ligne
      for (int j = 0; j < b.width; j++)
      {        
        // j représenta la colonne
        color pix = b.get(j,i); // coloration
        color ree = c.get(j,i); // pixel réel
        color gray = a.get(j,i); // profondeur
        if (reel) {
          fill(ree,255);
        } 
        else {
          fill(pix,255);
        }
        translate(j,i,red(gray));
        sphere(2);
        translate(-j,-i,-red(gray));
      }
    }
    // 
    println("3D terminée");
    mode = "rien";
  } 
  else {
  }
}

