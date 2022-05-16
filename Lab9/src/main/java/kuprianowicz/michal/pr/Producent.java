package kuprianowicz.michal.pr;

import java.util.Random;

public class Producent extends Thread{
    private final Warehouse warehouse;
    private final Product product;
    private final int id;

    Producent(Warehouse warehouse, Product product, int id)
    {
        this.warehouse = warehouse;
        this.product=product;
        this.id=id;
    }


    @Override
    public void run()
    {
        System.out.println("Producent: new Thread");
        System.out.println(this);

        while(true)
        {
            Random random=new Random();

            try
            {
                //waits 1-10s
                sleep((random.nextInt(10)+1)*1000);

                //amount = 1...10
                int amount =random.nextInt(30)+1;



                int output=warehouse.receiveProduct(this.product,amount);

                if(output==amount)
                {
                    System.out.println("Producent: Successfully produced "+amount+" "+this.product);
                }
                else if (output!=-1)
                {
                    System.out.println("Producent: Produced only "+ amount+" "+this.product);
                }
                else
                {
                    System.out.println("Producent: Production error");
                }



            }
            catch(Exception e)
            {
                System.err.println(e.getClass());
            }
        }

    }

    @Override
    public String toString() {
        return "Producent{" +
                "warehouse=" + warehouse +
                ", product=" + product +
                ", id=" + id +
                '}';
    }
}
