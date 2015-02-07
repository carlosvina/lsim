#ifndef PROMEDIO_HH
#define PROMEDIO_HH

using namespace std;

class bloques_muestras;

class Promedio
{
  bloques_muestras *mul_1, *mul_2;
  unsigned long samples;
  int autocorrelacionado;  

  void init_medida(float ftemp,	float ftemp2);
  void libera_promedio();
public:

  Promedio(float calidad=0.0, float tolerancia_relativa=0.1);
  // inicializa una variable de tipo promedio antes de dar comienzo a
  // la estimacion

  void computa(double muestra);
  // computa la muestra

  double estimacion();
  // devuelve el promedio hasta el momento

  float calidad();
  // devuelve la calidad hasta el momento en la estimacion de la
  // media

  static unsigned numero_promedios();
  // Devuelve el numero actual de objetos promedio que no han alcanzado
  // todavia los requisitos

  unsigned long num_muestras() const;
  // devuelve el numero de muestras computadas hasta el momento

  // a~nadir destructor (~Promedio) si necesario
  ~Promedio();

  void operator=(const Promedio& seg);
  Promedio(Promedio &seg);
};
#endif
