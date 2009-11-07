import java.util.ArrayList;

public class BipedeSequence
{
  private String name;

  private String filename;

  private int index;

  private ArrayList commandes;

  public String getName()
  {
    return name; 
  }

  public void setName(String n)
  {
    name = n; 
  }

  public String getFilename()
  {
    if (filename == null)
    {
      return "default"; 
    }
    return filename;
  }

  public BipedeCommande currentCommande()
  {
    return (BipedeCommande) commandes.get(index);
  }

  public BipedeCommande nextCommande()
  {
    if (index == commandes.size()-1)
    {
      addNewCommande();
    }
    index++;    
    return (BipedeCommande) commandes.get(index);    
  }

  public void addNewCommande()
  {
    BipedeCommande tmp = new BipedeCommande();
    commandes.add(tmp);
  }

  public void setCommandes(ArrayList al)
  {
    this.commandes = al;
  }

}





