import processing.core.*;


public class Marionnette {

  Articulation aNeckP;
  Articulation aNeckT;
  Articulation aEyesH;
  Articulation aEyesV;
  Articulation aMouth;
  
  public Marionnette() {
    aNeckP = new Articulation(-60,60,0);
    aNeckT = new Articulation(-45,45,0);    
    aEyesH = new Articulation(-45,45,0);
    aEyesV = new Articulation(-45,45,0);
    aMouth = new Articulation(0,90,0);
  }
  
  public String toString() {
    StringBuffer sb = new StringBuffer("");
    sb.append(aNeckP); 
    sb.append(aNeckT);
    sb.append(aEyesH);
    sb.append(aEyesV);
    sb.append(aMouth);
    return sb.toString();
  }

}

