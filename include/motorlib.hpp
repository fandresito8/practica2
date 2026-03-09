#ifndef MOTORLIB
#define MOTORLIB

#include "motorlib/monitor.hpp"
#include <glui/glui.h>

extern MonitorJuego monitor;

struct EnLinea{
  list<pair<int, int>> listaObjetivos;
  ubicacion posInicialIngeniero;
  ubicacion posInicialTecnico;
  string ubicacion_mapa;
  int semilla;
  int level;
  int max_instantes;
  int initial_energy;
  int eco_impact_threshold;
};


bool lanzar_motor_juego(int acc = -1);
void lanzar_motor_juego2(MonitorJuego &monitor);
void nucleo_motor_juego(MonitorJuego & monitor, int acc);
void lanzar_motor_grafico(int argc, char ** argv);
void lanzar_motor_grafico_verOnline(int argc, char ** argv, EnLinea &argumentos);

#endif
