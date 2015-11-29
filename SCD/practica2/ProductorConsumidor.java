/******************************************************************************
EJERCICIO 1. EL PROBLEMA DEL PRODUCTOR CONSUMIDOR
Montserrat Rodriguez Zamorano
Doble Grado en Ingenieria Informatica y Matematicas. Curso 2015/2016

Colores para la consola

public static final String ANSI_RESET = "\u001B[0m";
public static final String ANSI_BLACK = "\u001B[30m";
public static final String ANSI_RED = "\u001B[31m";
public static final String ANSI_GREEN = "\u001B[32m";
public static final String ANSI_YELLOW = "\u001B[33m";
public static final String ANSI_BLUE = "\u001B[34m";
public static final String ANSI_PURPLE = "\u001B[35m";
public static final String ANSI_CYAN = "\u001B[36m";
public static final String ANSI_WHITE = "\u001B[37m";

Para terminar, poner ANSI_RESET. Ejemplo

System.out.println(ANSI_RED + "This text is red!" + ANSI_RESET);

******************************************************************************/

import monitor.*;

class Buffer extends AbstractMonitor
{
  private int      numSlots = 0    ,
                   cont     = 0    ;
  private double[] buffer   = null ;
  private Condition productores = makeCondition();
  private Condition consumidores = makeCondition();

  public Buffer( int p_numSlots )
  {
    enter();
    numSlots = p_numSlots ;
    buffer = new double[numSlots] ;
    leave();
  }
  public void depositar( double valor )
  {
    enter();
    while( cont == numSlots )
      productores.await();
    buffer[cont] = valor ;
    cont++ ;
    consumidores.signal() ;
    leave();
  }
  public double extraer()
  {
    enter();
    double valor ;
    while( cont == 0 )
      consumidores.await() ;
    cont--;
    valor = buffer[cont] ;
    productores.signal();
    leave();
    return valor;
  }
}

// ****************************************************************************

class Productor implements Runnable
{
  private Buffer bb    ;
  private int    veces ,
                 numP  ;
  public  Thread thr   ;

  public Productor( Buffer pbb, int pveces, int pnumP )
  {
    bb    = pbb;
    veces = pveces;
    numP  = pnumP ;
    thr   = new Thread(this,"productor "+numP);
  }

  public void run()
  {
    try
    {
      double item = 100*numP ;

      for( int i=0 ; i < veces ; i++ )
      {
        System.out.println("\u001B[32m" +
        thr.getName()+", produciendo " + item + "\u001B[0m");
        bb.depositar( item++ );
      }
    }
    catch( Exception e )
    {
      System.err.println("Excepcion en main: " + e);
    }
  }
}

// ****************************************************************************

class Consumidor implements Runnable
{
  private Buffer  bb    ;
  private int     veces ,
                  numC  ;
  public  Thread  thr   ;

  public Consumidor( Buffer pbb, int pveces, int pnumC )
  {
    bb    = pbb;
    veces = pveces;
    numC  = pnumC ;
    thr   = new Thread(this,"consumidor "+numC);
  }
  public void run()
  {
    try
    {
      for( int i=0 ; i<veces ; i++ )
      {
        double item = bb.extraer ();
        System.out.println("\u001B[33m"+
        thr.getName()+", consumiendo "+item + "\u001B[0m");
      }
    }
    catch( Exception e )
    {
      System.err.println("Excepcion en main: " + e);
    }
  }
}

// ****************************************************************************

class MainProductorConsumidor
{
  public static void main( String[] args )
  {
    if ( args.length != 5 )
    {
      System.err.println("Uso: ncons nprod tambuf niterp niterc");
      return ;
    }

    // leer parametros, crear vectores y buffer intermedio
    Consumidor[] cons      = new Consumidor[Integer.parseInt(args[0])] ;
	  Productor[]  prod      = new Productor[Integer.parseInt(args[1])] ;
	  Buffer       buffer    = new Buffer(Integer.parseInt(args[2]));
	  int          iter_cons = Integer.parseInt(args[3]);
	  int          iter_prod = Integer.parseInt(args[4]);

    if ( cons.length*iter_cons != prod.length*iter_prod )
	  {
      System.err.println("no coinciden número de items a producir con a cosumir");
      return ;
    }

	  // crear hebras
	  for(int i = 0; i < cons.length; i++)
	    cons[i] = new Consumidor(buffer,iter_cons,i) ;
	  for(int i = 0; i < prod.length; i++)
	    prod[i] = new Productor(buffer,iter_prod,i) ;

	  // poner en marcha las hebras
	  for(int i = 0; i < prod.length; i++)
      prod[i].thr.start();
	  for(int i = 0; i < cons.length; i++)
      cons[i].thr.start();
  }
}
