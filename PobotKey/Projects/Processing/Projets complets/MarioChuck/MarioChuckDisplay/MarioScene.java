import processing.core.*;

public class MarioScene {

  PApplet p;  

  PImage[] images = new PImage[6];

  Figure[] figures = new Figure[6];

  int index = 0;

  public MarioScene(PApplet pa) {
    this.p = pa;

  }  

  public void sauvePosition(MarioSimu msi, String name) {
    if (index >= 6) {
      p.println("Error: no more free attitudes");
      return;
    }
    Figure fig = new Figure(msi.mario);
    fig.filename = name;
    figures[index] = fig;
    index++;
  }

  public void draw() {
    p.smooth();
    for (int i=0; i<6; i++) 
    {
      Figure fig = figures[i];
      if (fig != null) {
        // affichage du cadre
        //p.fill(0,30*i,0);
        //p.rect(10+85*i,10,75,75); 
        // affichage de l'image
        if (images[i] == null) {
          PImage pi = p.loadImage(fig.filename);
          pi.copy(110,110,250,250,0,0,520,520);
          images[i] = pi;
        }

        p.image(images[i], 10+85*i, 10, 75, 75);

      }      
    }    
    p.noSmooth();
  }

}



