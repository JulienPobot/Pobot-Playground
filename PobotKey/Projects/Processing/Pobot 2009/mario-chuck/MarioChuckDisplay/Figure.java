/**
 * Une figure particulière de la marionnette
 */
public class Figure {

  public String filename;

  public int positionPan;

  public int positionTilt;

  public int positionEyesH;

  public int positionEyesV;

  public int positionMouth;

  public Figure(Marionnette mario) {
    positionPan = mario.aNeckP.position;
    positionTilt = mario.aNeckT.position;
    positionEyesH = mario.aEyesH.position;
    positionEyesV = mario.aEyesV.position;
    positionMouth = mario.aMouth.position;
  }
}


