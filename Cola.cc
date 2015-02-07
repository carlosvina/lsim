#include "Cola.hh"


int Cola::numElementosAlfa;
int Cola::numElementosBeta;
int Cola::numAtendidos;
VarAleat Cola::tEntreLlegadas;
VarAleat Cola::tDemandado;

Cola::Cola(char **argv) {

    lista = NULL;
    ultimoElemento = NULL;
    ultimoAlfa = NULL;
    proximoNoAtendido = NULL;
    numElementosAlfa = 0;
    numElementosBeta = 0;
    numAtendidos = 0;
    prioridadBeta = 0;
    ultimaActualizacion = 0;
    tGlobal = 0;
    calidad = 0;
    tolRelativa = 0;
    for (int i = 0; i <= 2; ++i) {
        eventos[i] = new evento;
    }
    eventos[CAMBIO_COLA]->tEvento = MAX_DOUBLE;
    eventos[SALIDA]->tEvento = MAX_DOUBLE;
    procesarParametros(argv);

}

void Cola::procesarParametros(char** argv) {

    ifstream fichero;
    fichSalida = argv[1];
    fichero.open(argv[1]);
    fichero >> tolRelativa;
    fichero >> calidad;
    miPromedio = new Promedio(calidad, tolRelativa);
    fichero >> alfa;
    fichero >> beta;
    fichero >> disciplina;
    fichero >> numRecursos;
    fichero >> tEntreLlegadas;
    fichero >> tDemandado;
    tEntreLlegadas.gen.semilla(atoi(argv[2]));
    tDemandado.gen.semilla(atoi(argv[2]));
    tTransitorio = atof(argv[3]);
    fichero.close();
    eventos[LLEGADA]->tEvento = tGlobal + ++tEntreLlegadas;
    eventos[LLEGADA]->tarea = NULL;

}

void Cola::simula() {

    while (miPromedio->numero_promedios() != 0) {
        switch (proximoEvento()) {
            case LLEGADA:
                ++ * this;
                break;
            case SALIDA:
                -- * this;
                break;
            case CAMBIO_COLA:
                ! * this;
                break;
            default:
                cerr << "Explotó en el case!!" << endl;
                break;
        }
    }
    imprimeResultados(miPromedio->estimacion(), miPromedio->calidad());
}

void Cola::operator++() {
    nuevoElemento(++this->tDemandado);
    eventos[LLEGADA]->tEvento = tGlobal + ++this->tEntreLlegadas;
}

void Cola::operator--() {
    if (disciplina == RR) {
        if (actualizaRR(SALIDA)) {
            if (tGlobal > tTransitorio)
                miPromedio->computa(tGlobal - eventos[SALIDA]->tarea->muestra.iLlegada);
            eliminaElemento(eventos[SALIDA]->tarea);
            if (ultimoElemento != NULL) {
                eventos[SALIDA]->tEvento = ultimoElemento->muestra.salidaRR;
                eventos[SALIDA]->tarea = ultimoElemento;
            }
            if ((numRecursos > numAtendidos) && (ultimoAlfa != NULL)) {
                eventos[CAMBIO_COLA]->tEvento = tGlobal;
                eventos[CAMBIO_COLA]->tarea = ultimoAlfa;
            }
        }
    } else { // si FCFS
        if (tGlobal > tTransitorio)
            miPromedio->computa(tGlobal - eventos[SALIDA]->tarea->muestra.iLlegada);
        eliminaElemento(eventos[SALIDA]->tarea);
        salidaFCFS();
    }
}

void Cola::operator!() {

    if (disciplina == RR) {
        actualizaRR(CAMBIO_COLA);
        if (numElementosBeta == 0) prioridadBeta = tGlobal * (1 - alfa / beta) + ultimoAlfa->muestra.iLlegada * alfa / beta;
        ultimoAlfa->muestra.queCola = BETA;
        numElementosAlfa--;
        numElementosBeta++;
        numAtendidos++;
        ultimoAlfa->muestra.atendiendo = true;
        _lista *auxUltimoAlfa = ultimoAlfa;
        ultimoAlfa = ultimoAlfa->ant;
        ordenaRR(auxUltimoAlfa);
        double valor = (double) numAtendidos / (double) numRecursos;
        if (valor > 1) valor = 1;
        if (eventos[SALIDA]->tEvento > auxUltimoAlfa->muestra.tServicioDemandado / valor + tGlobal) {
            eventos[SALIDA]->tEvento = auxUltimoAlfa->muestra.tServicioDemandado / valor + tGlobal;
            eventos[SALIDA]->tarea = auxUltimoAlfa;
        }
        if (ultimoAlfa == NULL) {
            eventos[CAMBIO_COLA]->tEvento = MAX_DOUBLE;
        }
        if (numRecursos <= numAtendidos) {
            eventos[CAMBIO_COLA]->tEvento = instanteCambioCola();
        }

    } else { // si FCFS
        if (numElementosBeta == 0)
            prioridadBeta = tGlobal * (1 - alfa / beta) + ultimoAlfa->muestra.iLlegada * alfa / beta;
        ultimoAlfa->muestra.queCola = BETA;
        numElementosAlfa--;
        numElementosBeta++;
        ultimoAlfa = ultimoAlfa->ant;
        eventos[CAMBIO_COLA]->tEvento = instanteCambioCola();
        if (eventos[SALIDA]->tEvento == MAX_DOUBLE)
            eventos[SALIDA]->tEvento = eventos[SALIDA]->tarea->muestra.tServicioDemandado + tGlobal;
    }
}

void Cola::nuevoElemento(double muestra) {

    _lista *lista_aux = new _lista;
    lista_aux->muestra.tServicioDemandado = muestra;
    lista_aux->muestra.iLlegada = tGlobal;
    lista_aux->muestra.salidaRR = MAX_DOUBLE;
    if (lista == NULL) {
        lista = lista_aux;
        ultimoElemento = lista;
        lista->muestra.queCola = BETA;
        lista->muestra.atendiendo = true;
        lista->muestra.iAtendido = tGlobal;
        numElementosBeta = 1;
        numElementosAlfa = 0;
        numAtendidos = 1;
        ultimaActualizacion = tGlobal;
        prioridadBeta = tGlobal;
        eventos[SALIDA]->tarea = lista;
        eventos[SALIDA]->tEvento = lista->muestra.tServicioDemandado + tGlobal;
        lista_aux->sig = NULL;
        lista_aux->ant = NULL;
    } else { // si lista no vacia
        lista_aux->sig = lista;
        lista->ant = lista_aux;
        lista_aux->ant = NULL;
        lista = lista_aux;
        if ((numRecursos > numElementosBeta) || ((alfa == 0) && (beta == 0) && (disciplina == RR))) {
            lista->muestra.queCola = BETA;
            lista->muestra.atendiendo = true;
            lista->muestra.iAtendido = tGlobal;
            if (disciplina == RR) {
                actualizaRR(LLEGADA);
            } else { // si FCFS

                if (eventos[SALIDA]->tEvento > lista_aux->muestra.tServicioDemandado + tGlobal) {
                    eventos[SALIDA]->tEvento = lista_aux->muestra.tServicioDemandado + tGlobal;
                    eventos[SALIDA]->tarea = lista_aux;
                }
            }
            numElementosBeta++;
            numAtendidos++;
            if (disciplina == RR) ordenaRR(lista);

        }
        else { // llegan a alfa
            lista->muestra.queCola = ALFA;
            lista->muestra.atendiendo = false;
            if (numElementosAlfa == 0) {
                ultimoAlfa = lista;
                if (proximoNoAtendido == NULL) proximoNoAtendido = lista;
                eventos[CAMBIO_COLA]->tEvento = instanteCambioCola();
                eventos[CAMBIO_COLA]->tarea = ultimoAlfa;
            }
            numElementosAlfa++;
        }
    }

}

bool Cola::actualizaRR(int tipoEvento) {
    eventos[SALIDA]->tEvento = MAX_DOUBLE; // ponemos el valor por defecto
    eventos[SALIDA]->tarea = NULL;
    if (ultimoElemento == NULL) return false;

    _lista *aux = ultimoElemento;
    double decremento, valor = (double) numRecursos / (double) numAtendidos;
    bool salida = false;
    if (valor > 1) valor = 1;
    decremento = (tGlobal - ultimaActualizacion) * valor;
    for (int i = 0; i < numAtendidos; ++i) {
        aux->muestra.tServicioDemandado -= decremento;
        if (tipoEvento == SALIDA) aux->muestra.salidaRR = (aux->muestra.tServicioDemandado / valor) + tGlobal;
        else {
            valor = (double) numRecursos / (double) (numAtendidos + 1);
            if (valor > 1) valor = 1;
            aux->muestra.salidaRR = (aux->muestra.tServicioDemandado / valor) + tGlobal;
        }
        if (esCero(aux->muestra.tServicioDemandado)) {
            aux->muestra.tServicioDemandado = 0;
            aux->muestra.salidaRR = tGlobal;
            salida = true;
        }
        if (eventos[SALIDA]->tEvento > aux->muestra.salidaRR) {
            eventos[SALIDA]->tEvento = aux->muestra.salidaRR;
            eventos[SALIDA]->tarea = aux;
        }
        aux = aux->ant;
    }
    ultimaActualizacion = tGlobal;

    return salida;
}

void Cola::salidaFCFS() {
    eventos[SALIDA]->tEvento = MAX_DOUBLE;
    eventos[SALIDA]->tarea = NULL;
    if (ultimoElemento == NULL) return;

    _lista *aux = ultimoElemento;
    if (proximoNoAtendido != NULL) {
        proximoNoAtendido->muestra.atendiendo = true;
        numAtendidos++;
        proximoNoAtendido->muestra.iAtendido = tGlobal;
        if (proximoNoAtendido->muestra.queCola == ALFA) {
            eventos[CAMBIO_COLA]->tEvento = tGlobal;
            eventos[CAMBIO_COLA]->tarea = proximoNoAtendido;
        }
        proximoNoAtendido = proximoNoAtendido->ant;
    }

    for (int i = 0; i < numAtendidos; ++i) {
        if (eventos[SALIDA]->tEvento > aux->muestra.tServicioDemandado + aux->muestra.iAtendido) {
            eventos[SALIDA]->tEvento = aux->muestra.tServicioDemandado + aux->muestra.iAtendido;
            eventos[SALIDA]->tarea = aux;
        }
        aux = aux->ant;
    }
    //por si hay que corregir el error de precisión del double
    if (eventos[SALIDA]->tEvento < tGlobal) eventos[SALIDA]->tEvento = tGlobal;


    if (eventos[CAMBIO_COLA]->tEvento == eventos[SALIDA]->tEvento) {
        eventos[SALIDA]->tEvento = MAX_DOUBLE;
    }
}

void Cola::ordenaRR(_lista *elemento) {
    if (elemento->sig == NULL) return;
    _lista *aux = elemento->sig;
    for (int i = 0; i < numAtendidos - 1; ++i) {
        if (elemento->muestra.tServicioDemandado < aux->muestra.tServicioDemandado) {
            aux = aux->sig;
        } else break;
    }
    if (elemento->sig == aux) return; // si es el que mas va a tardar en salir nos vamos
    _lista *auxLista = lista->sig;
    if (aux == NULL) { // si sera el primero en salir...
        elemento->sig->ant = elemento->ant;
        if (elemento->ant != NULL) elemento->ant->sig = elemento->sig;
        elemento->ant = ultimoElemento;
        ultimoElemento->sig = elemento;
        elemento->sig = NULL;
        ultimoElemento = elemento;
    }
    else { // si hay que introducirlo en el medio...
        elemento->sig->ant = elemento->ant;
        if (elemento->ant != NULL) elemento->ant->sig = elemento->sig;
        aux->ant->sig = elemento;
        elemento->ant = aux->ant;
        aux->ant = elemento;
        elemento->sig = aux;
    }
    if (numElementosAlfa == 0) lista = auxLista; // si he modificado la cabeza 
    // de la lista ajusto el puntero
}

void Cola::eliminaElemento(_lista *eliminar) {

    _lista *eraser = ultimoElemento;
    if (eraser == eliminar) {
        if (ultimoElemento != lista) {
            eraser = eraser->ant;
            ultimoElemento = eraser;
            ultimoElemento->sig = NULL;
        } else {
            lista = NULL;
            ultimoElemento = NULL;
        }
    } else {
        eraser = eliminar->sig;
        eraser->ant = eliminar->ant;
        if (lista != eliminar) eliminar->ant->sig = eraser;
        else lista = lista->sig;
    }
    numAtendidos--;
    numElementosBeta--;
    delete eliminar;
}

double Cola::instanteCambioCola() {
    double valor;
    if (ultimoAlfa == NULL) return MAX_DOUBLE;
    if (alfa < beta) return MAX_DOUBLE;
    if (!esCero(alfa - beta)) {
        valor = ((ultimoAlfa->muestra.iLlegada) * alfa -
                prioridadBeta * beta) / (alfa - beta);
        if (valor < tGlobal) return tGlobal; // si han llegado muy juntos  y uno pasa a beta porque  quedó vacía
        else return valor; // la caja de servicio, la prioridad del segundo ha alcanzado la del que acabo de cambiar
    }
    else return MAX_DOUBLE;

}

void Cola::imprimeResultados(double estimacion, float calidad) {

    ofstream fich;

    fichSalida.erase(fichSalida.begin() + fichSalida.find('.', 0), fichSalida.end());
    fichSalida.append(".out");
    fich.open(fichSalida.c_str());
    fich << "La estimación del tiempo de respuesta (E(T)) es: " << estimacion << endl;
    fich << "La calidad es: " << calidad << endl;
    fich << "Número de muestras computadas: " << miPromedio->num_muestras() << endl;
    fich << "Intervalo (" << estimacion * (1 - tolRelativa) << "," << estimacion * (1 + tolRelativa) << ")" << endl;
    fich.close();
}

int Cola::proximoEvento() {
    double menor = eventos[0]->tEvento;
    int indice = 0;
    for (int i = 1; i <= 2; ++i) {
        if (eventos[i] != NULL) {
            if (menor > eventos[i]->tEvento) {
                menor = eventos[i]->tEvento;
                indice = i;
            }
        }
    }
    tGlobal = eventos[indice]->tEvento;
    return indice;
}

Cola::~Cola() {
    for (int i = 0; i <= 2; ++i) delete eventos[i];
    delete miPromedio;
}
