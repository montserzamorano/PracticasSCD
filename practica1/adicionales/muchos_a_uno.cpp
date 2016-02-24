// *********************************************************************
// SCD. Práctica 1.
//
// Problema de los fumadores. Los fumadores pueden ir a varios estancos
// distintos
//
// Montserrat Rodriguez Zamorano
// *********************************************************************

#include <iostream>
#include <cassert>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>      // incluye "time(....)"
#include <unistd.h>    // incluye "usleep(...)"
#include <stdlib.h>    // incluye "rand(...)" y "srand"
#include <utility>
#include <vector>

using namespace std ;

/**
Para evitar que algunos fumadores se queden sin fumar porque hay otros fumadores
que necesitan el mismo ingrediente que ellos, se necesitará implementar un
sistema de asignación de prioridades.

Como variables globales tenemos las mismas que en la versión anterior y además
un vector de prioridades, un vector de string con los nombres de los ingrendientes,
ampliar la variable sobre_el_mostrador a un vector (cada estanco tiene su
mostrador) y el array desbloquea que nos servirá para saber qué estanco puede
desbloquear a qué fumador.

El estanquero colocará un ingrediente sobre el mostrador y se lo ofrecerá
a aquel fumador con máxima prioridad. La prioridad de los fumadores que necesitan
el mismo ingrediente se aumentará en 1 y la del fumador escogido será 0.

Diferencias con muchos-a-muchos: la función inicializarEstancos que nos
sirve para saber a que estanco puede ir qué fumador.
DIFERENCIA IMPORTANTE: El acceso al vector de prioridades NO tiene que hacerse
en exclusión mutua, ya que esta vez, los estanqueros sólo pueden modificar a
los fumadores que acuden a su estanco.
**/

// -------------------Variables constantes y globales---------------------------
const unsigned num_fumadores = 6 ;
const unsigned num_estanqueros = 2 ;
unsigned long sobre_el_mostrador[num_estanqueros] ; //variable compartida que representa el ingrediente
static bool primero = true ;
vector < pair <int,int> > prioridad_fumadores ; //vector para asignar prioridades
vector <string> nombres;
//ingredientes y fumador al que le falta
pair <int, string> ingredientes[num_fumadores];
//array numero fumadores y estanco que le desbloquea
int desbloquea[num_fumadores] ;
// ------------------------------Semáforos--------------------------------------
sem_t
  puede_suministrar[num_estanqueros] ,
  puede_fumar[num_fumadores] ,
  mutex ;
// ------------------------------Funciones--------------------------------------
//en esta función tenemos que poner a qué estanco puede ir cada fumador
void inicializarEstancos(){
  desbloquea[0] = 1 ;
  desbloquea[1] = 1 ;
  desbloquea[2] = 1 ;
  desbloquea[3] = 0 ;
  desbloquea[4] = 0 ;
  desbloquea[5] = 0 ;
}
void inicializarColaPrioridad(){
  pair < int, int > par ;
  for(int i = 0 ; i < num_fumadores ; i++) {
    par.first = i ;
    par.second = i ;
    prioridad_fumadores.push_back(par) ;
  }
}
//Devuelve el índice del fumador que tiene máxima prioridad para
//recibir el ingrediente
int buscarPrioridad(int i, int e){
  int maximo = -1; //posicion de maxima prioridad
  int prioridad_actual ;
  for(int j = 0 ; j < prioridad_fumadores.size() ; j++){
    if(desbloquea[j] == e
       && ingredientes[prioridad_fumadores.at(j).first].second == nombres.at(i))
    {
      prioridad_actual = prioridad_fumadores.at(j).second ;
      if(maximo == -1){ //solo la primera vez
        maximo = j;
      }
      if( prioridad_actual > prioridad_fumadores.at(maximo).second )
        maximo = j ;
    }
  }
  prioridad_fumadores.at(maximo).second = 0; //quitamos la prioridad
  for(int j = 0; j < prioridad_fumadores.size() ; j++) {
    if(ingredientes[prioridad_fumadores.at(j).first].second == nombres.at(i)
      && j != maximo)
    {
      prioridad_fumadores.at(j).second++ ; //incrementamos en uno la prioridad
    }
  }
  return maximo ;
}
// función que simula la acción de fumar  como un retardo aleatorio de la hebra

void fumar()
{
   //  inicializa la semilla aleatoria  (solo la primera vez)
   static bool primera_vez = true ;
   if ( primera_vez )
   {   primera_vez = false ;
      srand( time(NULL) );
   }

   // calcular un numero aleatorio de milisegundos (entre 1/10 y 2 segundos)
   const unsigned miliseg = 100U + (rand() % 1900U) ;

   // retraso bloqueado durante 'miliseg' milisegundos
   usleep( 1000U*miliseg );
}
// ----------------------------------------------------------------------------
//función que simula la acción de suministrar ingredientes

int suministrar(int indice) {
  static bool primera_vez = true ;
  //  inicializa la semilla aleatoria  (solo la primera vez)
  if ( primera_vez )
  {   primera_vez = false ;
     srand( time(NULL) );
  }

  unsigned ing = rand() % 3 ;

  sem_wait( &mutex) ;
    cout << "El estanquero " << indice
    << " coloca en el mostrador " << nombres[ing] << "." << endl ;
  sem_post( &mutex) ;
  return ing ; //devuelve el índice del ingrediente correspondiente
}
// ----------------------------------------------------------------------------
//función "consumidor"

void * fumador(void * ih_void) {
  unsigned long ih = (unsigned long) ih_void ;
  while(true) {
    ///////////recoger/////////
    sem_wait ( &puede_fumar[ih]) ;
    sem_wait( &mutex) ;
      cout << "El fumador " << ingredientes[ih].first
      << " recoge " << ingredientes[ih].second << " del estanco "
      << desbloquea[ih] << "." << endl ;
    sem_post ( &mutex) ;
    /////////fumar////////////
    sem_wait( &mutex) ;
      cout << "El fumador " << ingredientes[ih].first
      << " empieza a fumar." << endl ;
    sem_post ( &mutex) ;
    sem_post ( &puede_suministrar[desbloquea[ih]]) ; //desbloquea al estanquero
    fumar() ;
    sem_wait( &mutex) ;
      cout << "El fumador " << ingredientes[ih].first
      << " ha terminado de fumar." << endl ;
    sem_post ( &mutex) ;
  }
	return NULL;
}
// ----------------------------------------------------------------------------
//función "productor"

void * estanquero(void * ih_void) {
  unsigned long ih = (unsigned long) ih_void ;
  int maxima_prioridad ;
  while(true) {
    sem_wait( &puede_suministrar[ih]) ;
      sobre_el_mostrador[ih] = suministrar(ih) ;
      maxima_prioridad = buscarPrioridad(sobre_el_mostrador[ih], ih) ;
    sem_post( &puede_fumar[maxima_prioridad]) ;
  }

  return NULL ;
}
// ----------------------------------------------------------------------------

int main()
{
  ////// fumadores y ingrediente que les falta ///////////////////////////
  pair <int,string> par = make_pair(1,"cerillas");
  ingredientes[0] = par ;
  par = make_pair(4, "cerillas") ;
  ingredientes[3] = par ;
  nombres.push_back("cerillas") ;
  par = make_pair(2, "tabaco") ;
  ingredientes[1] = par ;
  par = make_pair(5, "tabaco") ;
  ingredientes[4] = par ;
  nombres.push_back("tabaco") ;
  par = make_pair (3, "papel") ;
  ingredientes[2] = par ;
  par = make_pair (6, "papel") ;
  ingredientes[5] = par ;
  nombres.push_back("papel") ;
  ///////////////////////////////////////////////////////////////////////
  inicializarEstancos();
  inicializarColaPrioridad();
  for(int i = 0; i < num_estanqueros; i++) {
    sobre_el_mostrador[i] = -1 ;
  } //inicializar el array
  for( unsigned i = 0 ; i < num_estanqueros ; i++ ) {
    sem_init( &puede_suministrar[i], 0, 1) ;
  }
  for( unsigned i = 0 ; i < num_fumadores ; i++ ) {
    sem_init( &puede_fumar[i], 0, 0) ;
  }
  sem_init( &mutex, 0, 1) ;

  //crear las hebras
  pthread_t estanco[num_estanqueros], fumadores[num_fumadores] ;
  for( unsigned i = 0 ; i < num_estanqueros ; i++ ) {
    void * arg_ptr = (void *) i ; //convertir entero a puntero
    pthread_create( &estanco[i] , NULL, estanquero, arg_ptr ) ;
  }
  for( unsigned i = 0 ; i < num_fumadores ; i++ ) {
    void * arg_ptr = (void *) i ; //convertir entero a puntero
    pthread_create( &fumadores[i] , NULL, fumador, arg_ptr ) ;
  }

  //esperar a que las hebras se unan
  for( unsigned i = 0 ; i < num_fumadores ; i++ ) {
    pthread_join( fumadores[i] , NULL ) ;
  }
  for( unsigned i = 0 ; i < num_estanqueros ; i++ ) {
    pthread_join( estanco[i] , NULL ) ;
  }


  sem_wait( &mutex) ;
    cout << "Hasta mañana." << endl ;
  sem_post ( &mutex) ;

  //destruir los semáforos
  sem_destroy( &mutex) ;
  for( unsigned i = 0 ; i < num_fumadores ; i++ ) {
    sem_destroy( &puede_fumar[i]) ;
  }
  for( unsigned i = 0 ; i < num_estanqueros ; i++ ) {
    sem_destroy( &puede_suministrar[i]) ;
  }

  return 0 ;
}
