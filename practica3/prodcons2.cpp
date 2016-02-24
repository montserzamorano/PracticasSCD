/*****************************************************************************
* Montserrat Rodriguez Zamorano
* PRACTICA 3: Implementacion de algoritmos distribuidos con MPI
* Problema 1->Modificacion del productor consumidor
*****************************************************************************/

#include <mpi.h>
#include <iostream>
#include <math.h>
#include <time.h>      // incluye "time"
#include <unistd.h>    // incluye "usleep"
#include <stdlib.h>    // incluye "rand" y "srand"


#define Productor    0 //fuente si es el productor
#define Buffer       5 //tiene que ser el proceso 5
#define Consumidor   1 //fuente si es el Consumidor
#define ITERS       20
#define TAM          5
#define SIZ         10 //número de procesos
/*Plantilla colores*/
#define GREEN                   "\033[0;32m"
#define BLUE                    "\033[0;34m"
#define CYAN                    "\033[0;36m"
#define RED                     "\033[0;31m"
#define PURPLE                  "\033[0;35m"
#define GRAY                    "\033[0;37m"
#define DARK_GRAY               "\033[0;30m"
#define YELLOW                  "\033[0;33m"
#define WHITE                   "\033[0;37m"
#define DEFAULT                 "\033[0m"

using namespace std;

// ---------------------------------------------------------------------

void productor(int num_productor)
{
   int value ;

   for ( unsigned int i=num_productor; i < ITERS; i+=Buffer ){
      value = i ;
      cout << CYAN << "Productor " << num_productor << " produce valor " << value
      << endl << DEFAULT << flush;
      usleep( 1000U * (100U+(rand()%900U)) ); //bloqueo aleatorio
      //enviar valor, 1 elemento, tipo int, al buffer, con etiqueta indice, dentro
      //del comunicador universal
      MPI_Ssend( &value, 1, MPI_INT, Buffer, Productor, MPI_COMM_WORLD );
   }
}
// ---------------------------------------------------------------------

void buffer()
{
   int        value[TAM] ,
              peticion ,
              pos  =  0,
              rama ;
   MPI_Status status ;

   for( unsigned int i=0 ; i < ITERS*2 ; i++ )
   {
      if ( pos==0 )
         rama = 0 ;
      else if (pos==TAM)
         rama = 1 ;
      else
      {
         MPI_Probe( MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status );
         if ( status.MPI_TAG == Productor)
            rama = 0 ;
         else if(status.MPI_TAG == Consumidor)
            rama = 1 ;
      }
      switch(rama)
      {
         case 0:
            //si es el productor,...
            //sintaxis: (buf, num, datatype, source, tag, comm, status)
            MPI_Recv( &value[pos], 1, MPI_INT, MPI_ANY_SOURCE, Productor, MPI_COMM_WORLD, &status );
            cout << PURPLE << "Buffer recibe " << value[pos]
            << " de Productor " << status.MPI_SOURCE << "." << endl
            << DEFAULT << flush;
            pos++;
            break;
         case 1:
            //Si es el consumidor,...
            //recibir
            MPI_Recv( &peticion, 1, MPI_INT, MPI_ANY_SOURCE, Consumidor, MPI_COMM_WORLD, &status );
            cout << PURPLE "Consumidor " << status.MPI_SOURCE
            << " quiere leer." << endl << DEFAULT << flush;
            //sintaxis: (buf, num, datatype, dest, tag, comm)
            MPI_Ssend( &value[pos-1], 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
            cout << PURPLE "Buffer envía " << value[pos-1]
            << " a Consumidor " << status.MPI_SOURCE << "." << endl
            << DEFAULT << flush;
            pos--;
            break;
      }
   }
}

// ---------------------------------------------------------------------

void consumidor(int num_cons)
{
   int         value,
               peticion = 1 ;
   float       raiz ;
   MPI_Status  status ;

   for (unsigned int i=0;i<ITERS/(SIZ-Buffer-1);i++)
   {
      MPI_Ssend( &peticion, 1, MPI_INT, Buffer, Consumidor, MPI_COMM_WORLD );
      MPI_Recv ( &value, 1,    MPI_INT, Buffer, 0, MPI_COMM_WORLD,&status );
      cout << YELLOW << "Consumidor " << num_cons << " recibe valor "
      << value << " de Buffer " << endl << DEFAULT << flush;

      // espera bloqueado durante un intervalo de tiempo aleatorio
      // (entre una décima de segundo y un segundo)
      usleep( 1000U * (100U+(rand()%900U)) );

      raiz = sqrt(value) ;
   }
}
// ---------------------------------------------------------------------

int main(int argc, char *argv[])
{
   int rank,size;

   // inicializar MPI, leer identif. de proceso y número de procesos
   MPI_Init( &argc, &argv );
   MPI_Comm_rank( MPI_COMM_WORLD, &rank );
   MPI_Comm_size( MPI_COMM_WORLD, &size );

   // inicializa la semilla aleatoria:
   srand ( time(NULL) );

   // comprobar el número de procesos con el que el programa
   // ha sido puesto en marcha (debe ser 3)
   if ( size != SIZ )
   {
      cout<< "El numero de procesos debe ser " << SIZ <<endl;
      return 0;
   }

   // verificar el identificador de proceso (rank), y ejecutar la
   // operación apropiada a dicho identificador
   if ( rank < Buffer )
      productor(rank);
   else if ( rank == Buffer )
      buffer();
   else
      consumidor(rank);

   // al terminar el proceso, finalizar MPI
   MPI_Finalize( );
   return 0;
}
