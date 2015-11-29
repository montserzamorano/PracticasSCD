/******************************************************************************
EJERCICIO 2. EL PROBLEMA DE LOS FUMADORES
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

//monitor estanco
///////////////////// CLASE ESTANCO /////////////////////////////////////////

class Estanco extends AbstractMonitor{
  String [] arrayIngredientes = {"cerillas", "tabaco", "papel"};
  int num_fumadores = 3; //visibilidad de paquete
  private Condition[] condFumadores = new Condition[3];
  private Condition condEstanqueros;
  private int sobre_el_mostrador;
  public Estanco(){
    condEstanqueros=makeCondition(); //inicializar estanquero
    for(int i=0; i<num_fumadores; i++){ //inicializar fumadores
      condFumadores[i] = makeCondition();
    }
    sobre_el_mostrador=-1; //inicializar variable mostrador
  }
  //invocado por cada fumador, indicando su ingrediente o numero
  public void obtenerIngrediente(int miIngrediente){
    enter();

    if(sobre_el_mostrador != miIngrediente){  //si no hay ingrediente o es
                                              //distinto al suyo
      condFumadores[miIngrediente].await();   //esperar
    }
    System.out.println("\u001B[36m" +
    "Fumador " + (miIngrediente+1) + " recoge " +
    arrayIngredientes[miIngrediente] + "." + "\u001B[0m");
    sobre_el_mostrador=-1; //el mostrador queda vacío
    condEstanqueros.signal(); //liberar al estanquero

    leave();
  }
  //invocado por el estanquero, indicando el ingrediente que pone

  public void ponerIngrediente(int ingrediente){
    enter();

    sobre_el_mostrador = ingrediente; //ponerlo sobre el mostrador
    System.out.println("\u001B[33m" +
    "El estanquero coloca " + arrayIngredientes[ingrediente]
    + " sobre el mostrador" + "\u001B[0m");
    condFumadores[ingrediente].signal(); //liberar al fumador correspondiente

    leave();
  }
  //invocado por el estanquero

  public void esperarRecogidaIngrediente(){
    enter();

    if(sobre_el_mostrador != -1){ //si el mostrador no está vacío, dormir
      condEstanqueros.await();
    }

    leave();
  }
}
////////////////////////////FIN CLASE ESTANCO/////////////////////////////////

////////////////////////////CLASE FUMADOR/////////////////////////////////////

//hebra fumador
class Fumador implements Runnable {
  int miIngrediente;
  private Estanco estanco = new Estanco();
  public Thread thr; //objeto hebra encapsulado
  //constructor
  public Fumador(Estanco estanco, int p_miIngrediente){
    miIngrediente = p_miIngrediente;
    this.estanco = estanco;
    thr = new Thread(this,"Fumador" + (p_miIngrediente+1));
  } //Faltan parámetros
  public void run(){
    System.out.println("\u001B[36m" +
    thr.getName() + " llega al estanco." + "\u001B[0m");
    while(true)
    {
      estanco.obtenerIngrediente(miIngrediente); //coger el ingrediente
      //aux.dormir_max(2000);
      try{
        Thread.sleep(2000);
      }
      catch(InterruptedException e){
        System.err.println("Fumador interrumpido");
      }
    }
  }
}
/////////////////////////////FIN CLASE FUMADOR////////////////////////////////

////////////////////////////CLASE ESTANQUERO//////////////////////////////////
//hebra estanquero
class Estanquero implements Runnable {
  public Thread thr; //objeto hebra encapsulado
  private Estanco estanco = new Estanco();
  int ingrediente;
  //constructor
  public Estanquero(Estanco estanco){
    this.estanco=estanco;
    thr = new Thread(this,"Estanquero");
  }
  public void run(){
    System.out.println("\u001B[33m" +
    thr.getName() + " abre el estanco." + "\u001B[0m");
    while (true){
        ingrediente = (int) (Math.random() * 3.0); //0,1,2
        estanco.ponerIngrediente(ingrediente); //poner el ingrediente en el mostrador
        estanco.esperarRecogidaIngrediente(); //esperar a que el fumador lo recoja
    }
  }
}

///////////////////////// FIN CLASE ESTANQUERO ////////////////////////////////

/////////////////////////////// MAIN ///////////////////////////////////////

class MainFumadores
{
  public static void main( String[] args )
  {
    int num_fumadores = 3;
    Estanco estanco = new Estanco();
    Estanquero e = new Estanquero(estanco);
    Fumador[] f = new Fumador[num_fumadores];
    for(int i= 0; i < num_fumadores; i++){
      f[i] = new Fumador(estanco,i);
    }

	  // crear hebras
    e = new Estanquero(estanco) ;

	  for(int i = 0; i < f.length; i++){
	    f[i] = new Fumador(estanco,i);
    }

	  // poner en marcha las hebras
    e.thr.start();
	  for(int i = 0; i < f.length; i++)
      f[i].thr.start();
  }
}
