package kuprianowicz.michal.pr;


public class App 
{
    public static void main( String[] args )
    {

        Warehouse warehouse =new Warehouse();

        int producentsCounter=0;

        Producent breadProducent=new Producent(warehouse, Product.BREAD,++producentsCounter);

        Producent hamProducent=new Producent(warehouse, Product.HAM,++producentsCounter);

        Producent fishProducent=new Producent(warehouse, Product.FISH,++producentsCounter);

        int clientCounter=0;

        Client client1=new Client(warehouse,++clientCounter);
        Client client2=new Client(warehouse,++clientCounter);
        Client client3=new Client(warehouse,++clientCounter);


        warehouse.start();

        breadProducent.start();
        hamProducent.start();
        fishProducent.start();

        client1.start();
        client2.start();
        client3.start();

        System.out.println("Main thread starts");
    }
}
