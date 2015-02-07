#include "varaleat.hh"


//----------------------------------------------------------------------
// Implementaci�n de la clase Aleatorio
//----------------------------------------------------------------------

Aleatorio::Aleatorio(long sem) {
    //cout << "Llamada a constructor Aleatorio con semilla " << sem << endl;
    m = 0x7FFFFFFF;
    n_aleat = sem;

}

void Aleatorio::semilla(long sem) {
    // cout << "Llamada a metodo Aleatorio::semilla con argumento " << sem << endl;
    sem = sem;
    n_aleat = sem;
}

long Aleatorio::max(void) {
    // cout << "Llamada a Aleatorio::max" << endl;

    return 0x7FFFFFFF;
}

long Aleatorio::sig(void) {
    //cout<<"Llamada a Aleatorio::sig"<<endl;

    n_aleat = a * (n_aleat % q) - r * (n_aleat / q);
    if (n_aleat <= 0) {
        return n_aleat + m;
    }
    return n_aleat;

}

Aleatorio VarAleat::gen;

//----------------------------------------------------------------------
// Implementaci�n de la clase VarAleat
//----------------------------------------------------------------------

VarAleat::VarAleat(const char *tipo_va) {
    stringstream aux;
    // cout << "Llamada a constructor VarAleat con parametro " << tipo_va << endl;
    aux.str(tipo_va);
    aux >> *this; // el operador >> sobrecargado recibe un istream
    // que es lo que devuelve el aux.str()

}

double VarAleat::operator++() {
    //   cout << "Llamada a VarAleat operador ++" << endl;

    switch (this->tipo) {
        case 'D':
            return this->media; // devolvemos siempre el mismo valor
        case 'U': // generamos otro numero aleatorio y lo transformamos a U(a,b) y devolvemos  
            return (extremo_inf + ((extremo_sup - extremo_inf)*((double) gen.sig() / gen.m)));
        case 'N': // tenemos que generar 12 muestras U(0,1), sumarlas y restarle 6 al resultado
            // así tendremos una N(0,1)
            double muestra_u;
            muestra_u = 0;
            for (int i = 0; i < 12; i++) {
                muestra_u += ((double) gen.sig() / gen.m);
            }
            muestra_u -= 6;
            return ((muestra_u * desviacion) + media); // hay que convertir la N(0,1) a la N pedida

        case 'E': // E(x) = 1/lambda   
            return (-media * log(((double) gen.sig() / gen.m))); // muestra = -1/lambda * ln(muestra_unif)
        case 'H':
            // aqui se supone que la Fx(x) ya es el valor medio introducido por comandos no?
            double muestra_aux = 0, peso_acumulado = 0;
            muestra_aux = (double) gen.sig() / gen.m; //genero la muestra de uniforme(0,1) para elegir la distribución exponencial que usaré
            for (int i = 0; i < num_va_exp; i++) {
                peso_acumulado += peso_exp[i];
                if (muestra_aux <= peso_acumulado) { //escojo sacar muestra de la exponencial que corresponda
                    peso_acumulado = 0;
                    return (-media_exp[i] * log(((double) gen.sig()) / gen.m)); // debería salir siempre por aquí
                    break;
                }
            }

    }

    return -1;
}

istream & operator>>(istream& in, VarAleat& v) {
    in >> v.tipo;
    v.tipo = toupper(v.tipo);

    switch (v.tipo) {
        case 'D':
            in >> v.media;
            break;
        case 'U':
            in >> v.extremo_inf;
            in >> v.extremo_sup;
            break;
        case 'N':
            in >> v.media;
            in >> v.desviacion;
            break;
        case 'E':
            in >> v.media;
            break;
        case 'H':
            double aux;
            in >> v.num_va_exp;
            for (int i = 0; i < v.num_va_exp; i++) { // leemos media y peso de las
                in >> aux;
                v.media_exp.push_back(aux); // exponenciales
                in >> aux;
                v.peso_exp.push_back(aux);
            }

            break;

        default:
            cout << "No se ha encontrado una DUNEH..." << endl;

    }


}
