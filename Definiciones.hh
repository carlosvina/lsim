#ifndef _DEFINICIONES_HH
#define _DEFINICIONES_HH

#define ALFA 0
#define BETA 1
#define LLEGADA 0  // indica la 
#define SALIDA 1   // posicion del array
#define CAMBIO_COLA 2  // de los eventos
#define RR 'R'
#define FCFS 'F'

#define CERO_NEG -1e-4
#define CERO_POS 1e-4
#define MAX_DOUBLE 1.7976e308

#include <stdlib.h>
#include <iostream>
#include "Promedio.hh"
#include "varaleat.hh"


using namespace std;

typedef struct muestras {
    double tServicioDemandado;
    double iLlegada; // instante de llegada
    double iAtendido; // instante en el que se empieza a atender
    double salidaRR; // instante en el que se producira la salida en RR
    int queCola; // especifica si cola alfa o beta
    bool atendiendo; // indica si esta siendo servido
} muestras;

typedef struct _lista {
    muestras muestra;
    _lista *sig;
    _lista *ant;
} _lista;

typedef struct _evento {
    double tEvento; //instante en el que se produciré el evento
    _lista *tarea; //tarea relacionada al evento correspondiente
} evento;

// funciones

bool esCero(double x);



#endif  
