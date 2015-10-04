// *********************************************************************
// SCD. Ejemplos del seminario 1.
//
// Ejercicio calculo PI
// Montserrat Rodriguez Zamorano
// *********************************************************************

#include <iostream>
#include <pthread.h>
#include "fun_tiempo.h"

using namespace std ;

// ---------------------------------------------------------------------
// constante y variables globales (compartidas entre hebras)

const unsigned long m = long(1024)*long(1024) ;  // numero de muestras
const unsigned n      = 4 ;                      // numero de hebras

double resultado_parcial[n] ; // tabla de sumas parciales (una por hebra)

// ---------------------------------------------------------------------
// implementa función $f$

double f( double x )
{
   return 4.0/(1+x*x) ;     // $~~~~f(x)\,=\,4/(1+x^2)$
}
// ---------------------------------------------------------------------
// cálculo secuencial

double calcular_integral_secuencial( )
{
   double suma = 0.0 ;                      // inicializar suma
   for( unsigned long i = 0 ; i < m ; i++ ) // para cada $i$ entre $0$ y $m-1$
      suma += f( (i+0.5)/m );               // $~~~~~$ añadir $f(x_i)$ a la suma
                                            // actual
   return suma/m ;                          // devolver valor promedio de $f$
}
// ---------------------------------------------------------------------
// función que ejecuta cada hebra

void * funcion_hebra_contigua( void * ih_void )
{
   unsigned long ih = (unsigned long) ih_void ; // numero o indice de esta hebra
   double sumap = 0.0 ;
   // calcular suma parcial en "sumap"
   for(unsigned long i=ih*m/n; i<(ih+1)*m/n ; i++) {
     sumap += f((i+0.5)/m );
   }
   resultado_parcial[ih] = sumap ; // guardar suma parcial en vector.
   return NULL ;
}

void * funcion_hebra_entrelazada( void * ih_void )
{
   unsigned long ih = (unsigned long) ih_void ; // numero o indice de esta hebra
   double sumap = 0.0 ;
   // calcular suma parcial en "sumap"
   for(unsigned long i=ih; i<m ; i+=n) {
     sumap += f((i+0.5)/m );
   }
   resultado_parcial[ih] = sumap ; // guardar suma parcial en vector.
   return NULL ;
}

// ---------------------------------------------------------------------
// cálculo concurrente

double calcular_integral_concurrente_contigua( )
{
   // crear y lanzar $n$ hebras, cada una ejecuta "funcion\_concurrente"
   pthread_t hebras[n] ;
   for ( unsigned i = 0 ; i < n ; i++ ) {
     void * arg_ptr = (void *) i ; //convertir entero a puntero
     pthread_create ( & (hebras[i]), NULL, funcion_hebra_contigua, arg_ptr);
   }

   double resultado = 0; //guarda el resultado de cada hebra

   // esperar (join) a que termine cada hebra, sumar su resultado
   for ( unsigned i = 0 ; i < n; i++) {
     pthread_join( hebras[i] , NULL);
   }
   // devolver resultado completo
   for ( unsigned i = 0; i < n; i++) {
     resultado += resultado_parcial[i];
   }
   return resultado/m; //devolver valor promedio
}

double calcular_integral_concurrente_entrelazada(){
  // crear y lanzar $n$ hebras, cada una ejecuta "funcion\_concurrente"
  pthread_t hebras[n] ;
  for ( unsigned i = 0 ; i < n ; i++ ) {
    void * arg_ptr = (void *) i ; //convertir entero a puntero
    pthread_create ( & (hebras[i]), NULL, funcion_hebra_entrelazada, arg_ptr);
  }

  double resultado = 0; //guarda el resultado de cada hebra

  // esperar (join) a que termine cada hebra, sumar su resultado
  for ( unsigned i = 0 ; i < n; i++) {
    pthread_join( hebras[i] , NULL);
  }
  // devolver resultado completo
  for ( unsigned i = 0; i < n; i++) {
    resultado += resultado_parcial[i];
  }
  return resultado/m; //devolver valor promedio
}

// ---------------------------------------------------------------------

int main( int argc , char *argv[])
{
   cout << endl << "Ejemplo 4 : Cálculo de PI" << endl ;
   cout << "Número de hebras: " << n << endl ;
   double pi_sec = 0.0, pi_conc_ent = 0.0, pi_conc_cont=0.0 ;
   double tiempo_secuencial=0, tiempo_entrelazada=0, tiempo_contigua=0 ;
   int num_rep = 100;

   struct timespec inicio, fin ;

   for (unsigned i=0 ; i < num_rep ; i++ )
   {
     inicio = ahora() ;
     pi_sec  = calcular_integral_secuencial() ;
     fin = ahora() ;
     tiempo_secuencial += duracion(&inicio, &fin);

     inicio = ahora() ;
     pi_conc_ent = calcular_integral_concurrente_entrelazada() ;
     fin = ahora() ;
     tiempo_entrelazada += duracion(&inicio, &fin) ;

     inicio = ahora() ;
     pi_conc_cont = calcular_integral_concurrente_contigua() ;
     fin = ahora() ;
     tiempo_contigua += duracion(&inicio, &fin) ;
    }

   tiempo_secuencial = tiempo_secuencial/num_rep ;
   tiempo_entrelazada = tiempo_entrelazada/num_rep ;
   tiempo_contigua = tiempo_contigua/num_rep ;

   cout << endl << "1.Aproximación de PI" << endl;
   cout << "valor de PI (calculado secuencialmente)  == " << pi_sec  << endl
        << "valor de PI (calculado de forma entrelazada) == " << pi_conc_ent
        << endl
        << "valor de PI (calculado de forma contigua) ==" << pi_conc_cont
        << endl << endl ;

   cout << "2.Tiempos de ejecución" << endl;
   cout << "tiempo cálculo secuencial == " << tiempo_secuencial << endl
        << "tiempo cálculo entrelazada == " << tiempo_entrelazada << endl
        << "tiempo cálculo contigua == " << tiempo_contigua << endl << endl ;


   return 0 ;
}
// ---------------------------------------------------------------------
