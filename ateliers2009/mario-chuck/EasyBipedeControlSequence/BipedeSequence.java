import java.util.ArrayList;

public class BipedeSequence
{
  private String name;

  private String filename;

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
  
  public int nombreCommandes()
  {
   return commandes.size(); 
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
  
  public void removeCommande(int index)
  {
   this.commandes.remove(index); 
  }
  
  public BipedeCommande getCommande(int index)
  {
   return (BipedeCommande) this.commandes.get(index); 
  }

}





