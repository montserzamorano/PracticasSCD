// *********************************************************************
// SCD. Práctica 1.
//
// Ejercicio productor-consumidor (LIFO) múltiple
// Montserrat Rodriguez Zamorano
// *********************************************************************

#include <iostream>
#include <cassert>
#include <pthread.h>
#include <semaphore.h>

using namespace std ;

// ---------------------------------------------------------------------
// variables compartidas y constantes
// condicion : num_items_p*num_productores = num_items_c*num_consumidores = num_items
// num_items debe ser múltiplo de num_consumidores y num_productores
unsigned
  num_items_p = 0 ,
  num_items_c = 0 ;
const unsigned
  num_items = 30 ,
  num_consumidores = 3 ,
  num_productores = 3 ,
  tam_vector = 10 ;

int buffer [tam_vector] ; //buffer de escritura
unsigned primera_libre = 0 ;
unsigned primera_ocupada = 0 ;

sem_t
  mutex ,
  mutex_productores,
  mutex_consumidores,
  puede_leer ,
  puede_escribir ;
// ---------------------------------------------------------------------

unsigned ajustar() {
  num_items_p = num_items/num_productores ;
  num_items_c = num_items/num_consumidores ;
}

unsigned producir_dato()
{
  static int contador = 0 ;
  sem_wait (&mutex) ;
  cout << "producido: " << contador << endl << flush ;
  sem_post (&mutex) ;
  return contador++ ;
}
// ---------------------------------------------------------------------

void consumir_dato( int dato )
{
  sem_wait (&mutex) ;
    cout << "dato recibido: " << dato << endl ;
  sem_post (&mutex) ;
}
// ---------------------------------------------------------------------

void * productor( void * )
{
  for( unsigned i = 0 ; i < num_items_p ; i++ )
  {
    sem_wait(&mutex_productores);
    int dato = producir_dato() ;
    sem_wait(&puede_escribir) ;
      if(primera_libre < tam_vector) { // si se puede escribir en el buffer, producir
        buffer[primera_libre] = dato ;
        primera_libre++ ;

        if(primera_libre == tam_vector) {
          primera_libre = 0;
        }
      }
    sem_post(&puede_leer) ;
    sem_post(&mutex_consumidores) ;
  }
  return NULL ;
}
// ---------------------------------------------------------------------

void * consumidor( void * )
{
  for( unsigned i = 0 ; i < num_items_c ; i++ )
  {
    sem_wait(&puede_leer) ;
      int dato ;
      sem_wait(&mutex_consumidores);
      if(primera_ocupada < tam_vector) {
        dato = buffer[primera_ocupada];
        primera_ocupada++ ;

        if(primera_ocupada == tam_vector){
          primera_ocupada = 0;
        }
      }
    sem_post(&puede_escribir) ;
    sem_post(&mutex_productores) ;
    consumir_dato(dato) ;
  }
  return NULL ;
}
//----------------------------------------------------------------------

int main()
{
  ajustar() ;
  pthread_t productora[num_productores],consumidora[num_consumidores] ;
  // Inicializar los semáforos
  sem_init( &puede_leer , 0 , 0 ); // inicialmente no se puede leer
  sem_init( &puede_escribir , 0 , 1 ); // inicialmente se puede escribir
  sem_init( &mutex , 0, 1);
  sem_init( &mutex_productores , 0, 1);
  sem_init( &mutex_consumidores , 0, 1);

  // Crear las hebras
  for ( unsigned i = 0 ; i < num_productores ; i++) {
    pthread_create( &productora[i] , NULL, productor, NULL) ;
  }
  
  for ( unsigned i = 0 ; i < num_consumidores ; i++) {
    pthread_create( &consumidora[i], NULL, consumidor, NULL) ;
  }

  //Esperar a que las hebras terminen
  for ( unsigned i = 0 ; i < num_productores ; i++) {
    pthread_join( productora[i], NULL ) ;
  }

  for ( unsigned i = 0 ; i < num_consumidores ; i++) {
    pthread_join( consumidora[i], NULL ) ;
  }

  //Escribir "fin" cuando hayan acabado las dos hebras
  cout << "fin" << endl ;

  //Destruir los semáforos
  sem_destroy( &puede_leer ) ;
  sem_destroy( &puede_escribir ) ;
  sem_destroy( &mutex ) ;
  sem_destroy( &mutex_productores ) ;
  sem_destroy( &mutex_consumidores ) ;


   return 0 ;
}
