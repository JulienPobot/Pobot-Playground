

public class BipedeCommande
{
  // genou gauche
  public int gg;

  // hanche gauche
  public int hg;

  // genou droit
  public int gd;

  // hanche droite
  public int hd; 

  public BipedeCommande()
  {
    gg = 5;
    hg = 5;
    gd = 5;
    hd = 5; 
  }

  public String textValue()
  {
    return "("+gg+","+hg+","+gd+","+hd+")";
  }

}

