package kuprianowicz.michal.pr;

import java.util.Random;

public class Client extends Thread {
    private Warehouse warehouse = null;
    private int id;


    @Override
    public void run()
    {
        System.out.println("Client: new Thread:");
        System.out.println(this);

        while(true)
        {
            Random random=new Random();

            try
            {
                //waits 1-10s
                sleep((random.nextInt(10)+1)*1000);

                //amount = 1...10
                int amount =random.nextInt(4)+1;
                Product product;

                switch(random.nextInt(3)) {
                    case 0:
                        product = Product.BREAD;
                        break;
                    case 1:
                        product = Product.HAM;
                        break;
                    default:
                        product = Product.FISH;
                        break;
                }

                System.out.println("Client: Tried to buy "+ amount+" "+product);

                int output=warehouse.shipProduct(product,amount);


                if(output==amount)
                {
                    System.out.println("Client: Successfully bought "+amount+" of "+product);
                }
                else if (output==-1)
                {
                    System.out.println("Client: No "+product+" in magazine");
                }
                else
                {
                    System.out.println("Client: Bought only "+amount+" of "+product);
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
        return "Client{" +
                "magazine=" + warehouse +
                ", id=" + id +
                '}';
    }

    public Client(Warehouse warehouse, int id) {
        this.warehouse = warehouse;
        this.id = id;
    }
}
