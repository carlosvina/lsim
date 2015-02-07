#include "Definiciones.hh"

bool esCero(double x) {
    if ((CERO_NEG <= x) && (x <= CERO_POS)) return true;
    else return false;
}
