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
  
  public void insertCommande(int index)
  {
    BipedeCommande tmp = new BipedeCommande();
    commandes.add(index, tmp);        
  }
  
  public void removeCommande(int index)
  {
   if (index < commandes.size())
   {
   this.commandes.remove(index); 
   }
  }
  
  public BipedeCommande getCommande(int index)
  {
   if (index >= commandes.size()) {
    index = index-1; 
   }
   return (BipedeCommande) this.commandes.get(index); 
  }

}





