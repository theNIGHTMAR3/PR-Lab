package kuprianowicz.michal.pr;

import java.util.HashMap;
import java.util.Map;

public class Warehouse extends Thread
{
    private final Map<Product, Integer> warehouse;

    private final int[] maxCapacities;
    private final int[] actualAmounts;


    Warehouse()
    {
        //0 -> BREAD
        //1 -> HAM
        //2 -> fISH
        this.maxCapacities=new int[3];

        for(int i=0;i<3;i++)
            this.maxCapacities[i]=50;

        //0 for actualAmounts
        this.actualAmounts=new int[3];

        this.warehouse=new HashMap<>();
    }

    @Override
    public void run()
    {
        System.out.println("Warehouse: Warehouse starts");
    }

    //receives products from producents
    synchronized int receiveProduct(Product product,int amount)
    {
        int index=product.ordinal();

        if (this.warehouse.containsKey(product))
        {
            if (this.actualAmounts[index] + amount <= this.maxCapacities[index])
            {
                this.actualAmounts[index] += amount;
                int value = amount + this.warehouse.get(product);
                this.warehouse.remove(product);
                this.warehouse.put(product, value);
                printWarehouse();

                System.out.println("Warehouse: Added "+amount+" of "+product+" to warehouse");
                return amount;
            }
            else if (this.actualAmounts[index] !=this.maxCapacities[index])
            {
                int value = this.maxCapacities[index] - this.actualAmounts[index];
                this.actualAmounts[index] = this.maxCapacities[index];
                value += this.warehouse.get(product);
                this.warehouse.remove(product);
                this.warehouse.put(product, value);
                System.out.println("Warehouse: Added "+amount+" of "+product+" to warehouse. Warehouse is full.");

                printWarehouse();
                return value;
            }
            else {
                System.out.println("Warehouse: Storage for "+product+" is full");
                return -1;
            }
        }
        else
        {
            if (this.actualAmounts[index] + amount <= this.maxCapacities[index])
            {
                this.actualAmounts[index] += amount;
                this.warehouse.put(product, amount);

                System.out.println("Warehouse: Added "+amount+" of "+product+" to warehouse, which doesn't contain this product");
                return amount;
            }
            else if (this.actualAmounts[index]!=this.maxCapacities[index])
            {
                int value = this.maxCapacities[index] - this.actualAmounts[index];
                this.warehouse.put(product, value);
                this.actualAmounts[index] = this.maxCapacities[index];
                System.out.println("Warehouse: Added "+amount+" of "+product+" to warehouse, which doesnt contain this product. Warehouse is full");
                return value;
            }
            else
            {
                System.out.println("Warehouse: Storage for "+product+" is full");
                return -1;
            }

        }
    }

    //ships products to clients
    synchronized Integer shipProduct(Product product, int amount)
    {
        int index=product.ordinal();

        if (this.warehouse.containsKey(product))
        {
            if (this.warehouse.get(product) >= amount)
            {
                int value = this.warehouse.get(product);
                this.warehouse.remove(product);

                if(value-amount != 0)
                {
                    this.warehouse.put(product, value - amount);
                }
                this.actualAmounts[index] -= amount;
                System.out.println("Warehouse: Sold " + amount + " " + product);
                System.out.println("Warehouse: Warehouse has " + this.actualAmounts[index] + " "+ product+" left");
                return amount; // return true
            }
            else
            {
                Integer value = this.warehouse.get(product);
                this.warehouse.remove(product);
                System.out.println("Warehouse: Sold " + value + " " + product +". There is no more of this product in warehouse");
                System.out.println("Warehouse: Warehouse has " + this.actualAmounts[index] + " "+ product+" left");
                this.actualAmounts[index] -= value;
                return value; // return true
            }
        } else {
            System.out.println("Warehouse: There is none of " + product);
            return -1;
        }
    }

    private void printWarehouse() {
        int totalAmount=0;
        for(int i=0;i<3;i++)
            totalAmount+=actualAmounts[i];

        System.out.println("Warehouse: " + totalAmount+" products");
        for (Map.Entry<Product, Integer> entry : this.warehouse.entrySet())
        {
            System.out.println("\t"+entry.getKey() + ": " + entry.getValue());
        }
    }

}
