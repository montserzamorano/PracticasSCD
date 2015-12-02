/******************************************************************************
EJERCICIO 3. EL PROBLEMA DE LA BARBERIA
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

class Barberia extends AbstractMonitor{
  private Condition salaEspera;
  private Condition barbero;
  private Condition silla;
  public Barberia(){
    salaEspera = makeCondition();
    barbero = makeCondition();
    silla = makeCondition();
  }
  //invocado por los clientes para cortarse el pelo
  public void cortarPelo(int i){
    enter();
    if(!silla.isEmpty()){ //si el barbero esta ocupado
      System.out.println("\u001B[32m" + "El cliente " + i
      + " se sienta a esperar en la sala de espera."+ "\u001B[0m");
      salaEspera.await(); //esperar en la sala de espera
    }
    barbero.signal(); //liberar al barbero
    System.out.println("\u001B[32m" + "Cliente "
    + i + " entra a cortarse el pelo." + "\u001B[0m");
    silla.await(); //ocupar la silla

    leave();
  }
  //invocado por el barbero para esperar(si procede) a un nuevo cliente
  //y sentarlo para el cortar
  public void siguienteCliente(){
    enter();
    //si no hay nadie en la sala de espera ni en la silla, dormir
    if(salaEspera.isEmpty() && silla.isEmpty()){
      System.out.println("\u001B[33m" + "Parece que no hay nadie..." + "\u001B[0m");
      System.out.println("\u001B[33m" + "zZzzZzZZzzzZzZZ" + "\u001B[0m");
      barbero.await(); //zZzzZ
    }
    System.out.println("\u001B[36m"
    + "¡Que pase el siguiente cliente!" +"\u001B[0m");
    salaEspera.signal(); //dejamos pasar al siguiente

    leave();
  }
  //invocado por el barbero para indicar que ha terminado de cortar el pelo
  public void finCliente(){
    enter();

    System.out.println("\u001B[36m"
    + "Ya he terminado de afeitar al cliente." + "\u001B[0m");
    silla.signal(); //liberamos la silla

    leave();
  }
}

class Cliente implements Runnable{
  public Thread thr;
  private int num_cliente;
  private Barberia barberia;
  public Cliente(Barberia barberia,int num){
    num_cliente = num + 1;
    this.barberia = barberia;
    thr = new Thread(this,"cliente " + num_cliente);
  }
  public void run(){
    while(true){
      System.out.println("\u001B[35m" + "El " + thr.getName()
      + " ha entrado en la barbería." + "\u001B[0m");
      barberia.cortarPelo(num_cliente); //el cliente espera(si procede) y se corta el pelo
      try{
        Thread.sleep(2000000); //el cliente está fuera de la barbería un tiempo.
      }
      catch(InterruptedException e){
        System.err.println("Cliente interrumpido");
      }
    }
  }
}

class Barbero implements Runnable{
  public Thread thr;
  private Barberia barberia = new Barberia();
  public Barbero(Barberia barberia){
    this.barberia = barberia;
    thr = new Thread(this, "barbero");
  }
  public void run(){
    while(true){
      try{
        //barberia.siguienteCliente();
        Thread.sleep(2500); //el barbero esta cortando el pelo
        barberia.finCliente();
        barberia.siguienteCliente();
      }
      catch(InterruptedException e){
        System.err.println("Barbero interrumpido");
      }
    }
  }
}

class MainBarberia
{
  public static void main( String[] args )
  {
    final int num_clientes = 10;
    Barberia barberia = new Barberia();
    Barbero b = new Barbero(barberia);
    Cliente[] c = new Cliente[num_clientes];
    for(int i= 0; i < num_clientes; i++){
      c[i] = new Cliente(barberia,i);
    }

    // crear hebras

    for(int i = 0; i < c.length; i++){
      c[i] = new Cliente(barberia,i);
    }

    // poner en marcha las hebras
    for(int i = 0; i < c.length; i++)
      c[i].thr.start();

    b.thr.start();
  }
}
