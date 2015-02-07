#include "Definiciones.hh"

#ifndef COLA_HH
#define	COLA_HH

class Cola {
public:

    Cola(char** argv);

    void simula();

    ~Cola();

private:

    // lista apunta al último elemento introducido en la cola
    // ultimoElemento apunta al final de la lista (donde estan los beta a punto de salir)
    _lista *lista, *ultimoElemento, *ultimoAlfa; // apunta al próximo que pasará a beta
    _lista *proximoNoAtendido; // proximo elemento a ser atendido (FCFS)
    int static numElementosAlfa, numElementosBeta, numAtendidos;
    int numRecursos;
    VarAleat static tEntreLlegadas, tDemandado;
    Promedio *miPromedio;
    double tGlobal; // tiempo global del sistema
    double ultimaActualizacion; // ultimo momento en que se actualizaron los tServicioDemandado (en RR)
    double prioridadBeta; // instante de llegada mas antiguo a beta, usado para recalcular la prioridad de beta.
    evento *eventos[3]; // contiene los tres próximos eventos de distinto tipo
    float tolRelativa, calidad, alfa, beta, tTransitorio;
    char disciplina; // R = RR, F = FCFS
    string fichSalida;

    // procesa la linea de comandos
    void procesarParametros(char** argv);

    // añade un nuevo elemento a la cola
    void nuevoElemento(double muestra);

    // elimina un elemento de la cola
    void eliminaElemento(_lista *eliminar);

    // devuelve el instante de tiempo en el que se produce un cambio de cola alfa a cola beta
    double instanteCambioCola();

    // devuelve el próximo evento de salida en FCFS
    void salidaFCFS();

    //ordena la salida en RR
    void ordenaRR(_lista *elemento);

    // actualiza los tServicioDemandado en RR y devuelve true si hay evento de salida
    bool actualizaRR(int tipoEvento);

    // introduce un nuevo elemento en la cola
    void operator++();

    // saca un elemento de la cola
    void operator--();

    // realiza un cambio de cola alfa a cola beta 
    void operator!();

    // imprime los resultados en un fichero
    void imprimeResultados(double estimacion, float calidad);

    // devuelve el índice del array del próximo evento y actualiza el tGlobal
    int proximoEvento();
};

#endif	/* COLA_HH */

