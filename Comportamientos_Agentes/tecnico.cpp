#include "tecnico.hpp"
#include "motorlib/util.h"
#include <iostream>
#include <queue>
#include <set>

using namespace std;

// =========================================================================
// ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
// =========================================================================

Action ComportamientoTecnico::think(Sensores sensores)
{
  Action accion = IDLE;

  switch (sensores.nivel) {
    case 0:
      accion = ComportamientoTecnicoNivel_0(sensores); break;
    case 1:
      accion = ComportamientoTecnicoNivel_1(sensores); break;
    case 2:
      accion = ComportamientoTecnicoNivel_2(sensores); break;
    case 3:
      accion = ComportamientoTecnicoNivel_E (sensores); break;
      //accion = ComportamientoTecnicoNivel_3(sensores); break;
    case 4:
      accion = ComportamientoTecnicoNivel_4(sensores); break;
    case 5:
      accion = ComportamientoTecnicoNivel_5(sensores); break;
    case 6:
      accion = ComportamientoTecnicoNivel_6(sensores); break;
  }

  return accion;
}

int VeoCasillaInteresanteT(char i, char c, char d, bool zap, int vis_i, int vis_c, int vis_d)
{
    if (c == 'U') return 2;
    else if (i == 'U') return 1;
    else if (d == 'U') return 3;
    else if (!zap) {
        if (c == 'D') return 2;
        else if (i == 'D') return 1;
        else if (d == 'D') return 3;
    }

    bool ci = (i == 'C' || i == 'D');
    bool cc = (c == 'C' || c == 'D');
    bool cd = (d == 'C' || d == 'D');

    if (!ci && !cc && !cd) return 0;

    int vi = ci ? vis_i : INT_MAX;
    int vc = cc ? vis_c : INT_MAX;
    int vd = cd ? vis_d : INT_MAX;

    if (vc <= vi && vc <= vd) return 2;
    else if (vi <= vd)        return 1;
    else                      return 3;
}

char ViablePorAlturaT(char casilla, int dif)
{
  if (abs(dif) <= 1) return casilla;
  else return 'P';
}

// Niveles del técnico
Action ComportamientoTecnico::ComportamientoTecnicoNivel_0(Sensores sensores) {
  Action accion = IDLE;
  ActualizarMapa(sensores);

  if (mapa_visitas.empty()) {
    mapa_visitas = vector<vector<int>>(mapaResultado.size(), vector<int>(mapaResultado[0].size(), 0));
  }

  mapa_visitas[sensores.posF][sensores.posC]++;

  if (sensores.superficie[0] == 'D') {
    tiene_zapatillas = true;
  }

  if (sensores.superficie[0] == 'U') {
    return IDLE;
  }

  char i = ViablePorAlturaT(sensores.superficie[1], sensores.cota[1]-sensores.cota[0]);
  char c = ViablePorAlturaT(sensores.superficie[2], sensores.cota[2]-sensores.cota[0]);
  char d = ViablePorAlturaT(sensores.superficie[3], sensores.cota[3]-sensores.cota[0]);

  ubicacion actual;
  actual.f = sensores.posF;
  actual.c = sensores.posC;
  actual.brujula = sensores.rumbo;

  ubicacion pos_i = Izquierda(actual);
  ubicacion pos_c = Delante(actual);
  ubicacion pos_d = Derecha(actual);

  int vis_i = mapa_visitas[pos_i.f][pos_i.c];
  int vis_c = mapa_visitas[pos_c.f][pos_c.c];
  int vis_d = mapa_visitas[pos_d.f][pos_d.c];

  if (sensores.agentes[2] == 'i' || c == 'P') { c = 'P'; vis_c = INT_MAX; }
  if (sensores.agentes[1] == 'i' || i == 'P') { i = 'P'; vis_i = INT_MAX; }
  if (sensores.agentes[3] == 'i' || d == 'P') { d = 'P'; vis_d = INT_MAX; }

  int pos = VeoCasillaInteresanteT(i, c, d, tiene_zapatillas, vis_i, vis_c, vis_d);

  switch (pos) {
    case 2: accion = WALK;     break;
    case 1: accion = TURN_SL;  break;
    case 3: accion = TURN_SR;  break;
    default: accion = TURN_SL; break;
  }

  if (accion == TURN_SL && last_action == accion) {
    giros_consecutivos++;
  } else {
    giros_consecutivos = 0;
  }

  if (giros_consecutivos >= 2) {
    giros_consecutivos = 0;
    pasos_evasion = 3;
    if (last_action == TURN_SL) gira_izq = true;
    else gira_izq = false;
  }

  if (pasos_evasion > 0) {
    pasos_evasion--;
    if (gira_izq) accion = TURN_SR;
    else accion = TURN_SL;
  }

  last_action = accion;
  return accion;
}

/**
 * @brief Comprueba si una celda es de tipo camino transitable.
 * @param c Carácter que representa el tipo de superficie.
 * @return true si es camino ('C'), zapatillas ('D') o meta ('U').
 */
bool ComportamientoTecnico::es_camino(unsigned char c) const {
  return (c == 'C' || c == 'D' || c == 'U');
}

int VeoCasillaInteresanteT_N1(char i, char c, char d, bool zap, int vis_i, int vis_c, int vis_d)
{
  if (!zap) {
    if (c == 'D') return 2;
    else if (i == 'D') return 1;
    else if (d == 'D') return 3;
  }

  bool ci = (i == 'C' || i == 'S' || i == 'D' || i == 'U');
  bool cc = (c == 'C' || c == 'S' || c == 'D' || c == 'U');
  bool cd = (d == 'C' || d == 'S' || d == 'D' || d == 'U');

  if (!ci && !cc && !cd) return 0;

  int vi = ci ? (i == 'S' ? vis_i * 2 : vis_i) : INT_MAX;
  int vc = cc ? (c == 'S' ? vis_c * 2 : vis_c) : INT_MAX;
  int vd = cd ? (d == 'S' ? vis_d * 2 : vis_d) : INT_MAX;

  if (vc <= vi && vc <= vd) return 2;
  else if (vi <= vd)        return 1;
  else                      return 3;
}

/**
 * @brief Comportamiento reactivo del técnico para el Nivel 1.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_1(Sensores sensores) {
  Action accion = IDLE;
  ActualizarMapa(sensores);

  if (mapa_visitas.empty()) {
    mapa_visitas = vector<vector<int>>(mapaResultado.size(), vector<int>(mapaResultado[0].size(), 0));
  }

  mapa_visitas[sensores.posF][sensores.posC]++;

  if (sensores.superficie[0] == 'D') {
    tiene_zapatillas = true;
  }

  char i = ViablePorAlturaT(sensores.superficie[1], sensores.cota[1]-sensores.cota[0]);
  char c = ViablePorAlturaT(sensores.superficie[2], sensores.cota[2]-sensores.cota[0]);
  char d = ViablePorAlturaT(sensores.superficie[3], sensores.cota[3]-sensores.cota[0]);

  ubicacion actual;
  actual.f = sensores.posF;
  actual.c = sensores.posC;
  actual.brujula = sensores.rumbo;

  ubicacion pos_i = Izquierda(actual);
  ubicacion pos_c = Delante(actual);
  ubicacion pos_d = Derecha(actual);

  int vis_i = mapa_visitas[pos_i.f][pos_i.c];
  int vis_c = mapa_visitas[pos_c.f][pos_c.c];
  int vis_d = mapa_visitas[pos_d.f][pos_d.c];

  if (sensores.agentes[2] == 'i' || c == 'P') { c = 'P'; vis_c = INT_MAX; }
  if (sensores.agentes[1] == 'i' || i == 'P') { i = 'P'; vis_i = INT_MAX; }
  if (sensores.agentes[3] == 'i' || d == 'P') { d = 'P'; vis_d = INT_MAX; }

  int pos = VeoCasillaInteresanteT_N1(i, c, d, tiene_zapatillas, vis_i, vis_c, vis_d);

  switch (pos) {
    case 2: accion = WALK;     break;
    case 1: accion = TURN_SL;  break;
    case 3: accion = TURN_SR;  break;
    default: accion = TURN_SL; break;
  }

  if (accion == TURN_SL && last_action == accion) {
    giros_consecutivos++;
  } else {
    giros_consecutivos = 0;
  }

  if (giros_consecutivos >= 2) {
    giros_consecutivos = 0;
    pasos_evasion = 3;
    if (last_action == TURN_SL) gira_izq = true;
    else gira_izq = false;
  }

  if (pasos_evasion > 0) {
    pasos_evasion--;
    if (gira_izq) accion = TURN_SR;
    else accion = TURN_SL;
  }

  last_action = accion;
  return accion;
}

/**
 * @brief Comportamiento del técnico para el Nivel 2.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_2(Sensores sensores) {
  return IDLE;
}

bool FindL(const NodoT &st, const list<NodoT> &lista) {
  auto it = lista.begin();
  while (it != lista.end() and !((*it) == st)) {
    it++;
  }
  return (it != lista.end());
}

bool FindS(const NodoT &st, const set<NodoT> &set) {
  auto it = set.begin();
  while (it != set.end() and !((*it) == st)) {
    it++;
  }
  return (it != set.end());
}

EstadoT NextCasillaTécnico(const EstadoT &st) {
  EstadoT siguiente = st;
  switch (st.site.brujula) {
    case norte:     siguiente.site.f = st.site.f - 1; break;
    case noreste:   siguiente.site.f = st.site.f - 1;
    siguiente.site.c = st.site.c + 1; break;
    case este:      siguiente.site.c = st.site.c + 1; break;
    case sureste:   siguiente.site.f = st.site.f + 1;
    siguiente.site.c = st.site.c + 1; break;
    case sur:       siguiente.site.f = st.site.f + 1; break;
    case suroeste:  siguiente.site.f = st.site.f + 1;
    siguiente.site.c = st.site.c - 1; break;
    case oeste:     siguiente.site.c = st.site.c - 1; break;
    case noroeste:  siguiente.site.f = st.site.f - 1;
    siguiente.site.c = st.site.c - 1; break;
  }
  return siguiente;
}

bool CasillaAccesibleTécnico(const EstadoT &st,
                              const vector<vector<unsigned char>> &terreno,
                              const vector<vector<unsigned char>> &altura) {
  EstadoT next = NextCasillaTécnico(st);
  bool check1 = terreno[next.site.f][next.site.c] != 'P' and
                terreno[next.site.f][next.site.c] != 'M';
  bool check2 = terreno[next.site.f][next.site.c] != 'B' or
               (terreno[next.site.f][next.site.c] == 'B' and st.zapatillas);
  bool check3 = abs((int)altura[next.site.f][next.site.c] -
                    (int)altura[st.site.f][st.site.c]) <= 1;
  return check1 and check2 and check3;
}

EstadoT applyT(Action accion, const EstadoT &st,
               const vector<vector<unsigned char>> &terreno,
               const vector<vector<unsigned char>> &altura) {
  EstadoT next = st;
  switch (accion) {
    case WALK:
      if (CasillaAccesibleTécnico(st, terreno, altura)) {
        next = NextCasillaTécnico(st);
      }
    break;
    case TURN_SR:
      next.site.brujula = (Orientacion)((next.site.brujula + 1) % 8);
    break;
    case TURN_SL:
      next.site.brujula = (Orientacion)((next.site.brujula + 7) % 8);
    break;
  }
  return next;
}

list<Action> ComportamientoTecnico::B_Anchura(const EstadoT &inicio, const EstadoT &fin,
                        const vector<vector<unsigned char>> &terreno,
                        const vector<vector<unsigned char>> &altura) {

    NodoT current_node;
    current_node.estado = inicio;
    current_node.secuencia = {};

    list<NodoT> frontier;
    list<NodoT> explored;
    list<Action> path;

    bool SolutionFound = (current_node.estado.site.f == fin.site.f &&
                          current_node.estado.site.c == fin.site.c);

    frontier.push_back(current_node);

    while (!frontier.empty() and !SolutionFound) {
        frontier.pop_front();
        explored.push_back(current_node);

        NodoT child_Walk;
        child_Walk.estado = applyT(WALK, current_node.estado, terreno, altura);
        child_Walk.secuencia = current_node.secuencia;

        // Comprobar si WALK llega a la solución
        if (child_Walk.estado.site.f == fin.site.f &&
            child_Walk.estado.site.c == fin.site.c) {
            child_Walk.secuencia.push_back(WALK);
            current_node = child_Walk;
            SolutionFound = true;
        } else if (!FindL(child_Walk, explored) and !FindL(child_Walk, frontier)) {
            child_Walk.secuencia.push_back(WALK);
            frontier.push_back(child_Walk);
        }

        if (!SolutionFound) {
            NodoT child_Turn_SR;
            child_Turn_SR.estado = applyT(TURN_SR, current_node.estado, terreno, altura);
            child_Turn_SR.secuencia = current_node.secuencia;

            if (!FindL(child_Turn_SR, explored) and !FindL(child_Turn_SR, frontier)) {
                child_Turn_SR.secuencia.push_back(TURN_SR);
                frontier.push_back(child_Turn_SR);
            }

            NodoT child_Turn_SL;
            child_Turn_SL.estado = applyT(TURN_SL, current_node.estado, terreno, altura);
            child_Turn_SL.secuencia = current_node.secuencia;

            if (!FindL(child_Turn_SL, explored) and !FindL(child_Turn_SL, frontier)) {
                child_Turn_SL.secuencia.push_back(TURN_SL);
                frontier.push_back(child_Turn_SL);
            }
        }

        if (!SolutionFound and !frontier.empty()) {
            current_node = frontier.front();
        }
    }

    if (SolutionFound)
        path = current_node.secuencia;

    return path;
}

list<Action> ComportamientoTecnico::B_Anchura_V2(const EstadoT &inicio, const EstadoT &fin,
                        const vector<vector<unsigned char>> &terreno,
                        const vector<vector<unsigned char>> &altura) {

    NodoT current_node;
    current_node.estado = inicio;
    current_node.secuencia = {};

    list<NodoT> frontier;
    set<NodoT> explored;
    list<Action> path;

    bool SolutionFound = (current_node.estado.site.f == fin.site.f &&
                          current_node.estado.site.c == fin.site.c);

    frontier.push_back(current_node);

    while (!frontier.empty() and !SolutionFound) {
        frontier.pop_front();
        explored.insert(current_node);

        NodoT child_Walk;
        child_Walk.estado = applyT(WALK, current_node.estado, terreno, altura);
        child_Walk.secuencia = current_node.secuencia;

        // Comprobar si WALK llega a la solución
        if (child_Walk.estado.site.f == fin.site.f &&
            child_Walk.estado.site.c == fin.site.c) {
            child_Walk.secuencia.push_back(WALK);
            current_node = child_Walk;
            SolutionFound = true;
        } else if (!FindS(child_Walk, explored)) {
            child_Walk.secuencia.push_back(WALK);
            frontier.push_back(child_Walk);
        }

        if (!SolutionFound) {
            NodoT child_Turn_SR;
            child_Turn_SR.estado = applyT(TURN_SR, current_node.estado, terreno, altura);
            child_Turn_SR.secuencia = current_node.secuencia;

            if (!FindS(child_Turn_SR, explored)) {
                child_Turn_SR.secuencia.push_back(TURN_SR);
                frontier.push_back(child_Turn_SR);
            }

            NodoT child_Turn_SL;
            child_Turn_SL.estado = applyT(TURN_SL, current_node.estado, terreno, altura);
            child_Turn_SL.secuencia = current_node.secuencia;

            if (!FindS(child_Turn_SL, explored)) {
                child_Turn_SL.secuencia.push_back(TURN_SL);
                frontier.push_back(child_Turn_SL);
            }
        }

        if (!SolutionFound and !frontier.empty()) {
            current_node = frontier.front();
        }
    }

    if (SolutionFound)
        path = current_node.secuencia;

    return path;
}

/**
* @brief Comportamiento del técnico para el Nivel E.
* @param sensores Datos actuales de los sensores.
* @return Acción a realizar.
*/
Action ComportamientoTecnico::ComportamientoTecnicoNivel_E(Sensores sensores) {
  Action accion = IDLE;
  if (!hayPlan) {
    EstadoT inicio, fin;
    inicio.site.f = sensores.posF;
    inicio.site.c = sensores.posC;
    inicio.site.brujula = sensores.rumbo;
    inicio.zapatillas = tiene_zapatillas;
    fin.site.f = sensores.BelPosF;
    fin.site.c = sensores.BelPosC;
    plan = B_Anchura_V2(inicio, fin, mapaResultado, mapaCotas);
    VisualizaPlan(inicio.site,plan);
    hayPlan = plan.size() != 0 ;
  }
  if (hayPlan and plan.size()>0) {
    accion = plan.front();
    plan.pop_front();
  }
  if (plan.size()== 0) {
    hayPlan = false;
  }
  return accion;
}

/**
 * @brief Comportamiento del técnico para el Nivel 3.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_3(Sensores sensores) {
  return IDLE;
}

/**
 * @brief Comportamiento del técnico para el Nivel 4.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_4(Sensores sensores) {
  return IDLE;
}

/**
 * @brief Comportamiento del técnico para el Nivel 5.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_5(Sensores sensores) {
  return IDLE;
}

/**
 * @brief Comportamiento del técnico para el Nivel 6.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_6(Sensores sensores) {
  return IDLE;
}




// =========================================================================
// FUNCIONES PROPORCIONADAS
// =========================================================================

/**
 * @brief Actualiza el mapaResultado y mapaCotas con la información de los sensores.
 * @param sensores Datos actuales de los sensores.
 */
void ComportamientoTecnico::ActualizarMapa(Sensores sensores) {
  mapaResultado[sensores.posF][sensores.posC] = sensores.superficie[0];
  mapaCotas[sensores.posF][sensores.posC] = sensores.cota[0];

  int pos = 1;
  switch (sensores.rumbo) {
    case norte:
      for (int j = 1; j < 4; j++)
        for (int i = -j; i <= j; i++) {
          mapaResultado[sensores.posF - j][sensores.posC + i] = sensores.superficie[pos];
          mapaCotas[sensores.posF - j][sensores.posC + i] = sensores.cota[pos++];
        }
      break;
    case noreste:
      mapaResultado[sensores.posF - 1][sensores.posC] = sensores.superficie[1];
      mapaCotas[sensores.posF - 1][sensores.posC] = sensores.cota[1];
      mapaResultado[sensores.posF - 1][sensores.posC + 1] = sensores.superficie[2];
      mapaCotas[sensores.posF - 1][sensores.posC + 1] = sensores.cota[2];
      mapaResultado[sensores.posF][sensores.posC + 1] = sensores.superficie[3];
      mapaCotas[sensores.posF][sensores.posC + 1] = sensores.cota[3];
      mapaResultado[sensores.posF - 2][sensores.posC] = sensores.superficie[4];
      mapaCotas[sensores.posF - 2][sensores.posC] = sensores.cota[4];
      mapaResultado[sensores.posF - 2][sensores.posC + 1] = sensores.superficie[5];
      mapaCotas[sensores.posF - 2][sensores.posC + 1] = sensores.cota[5];
      mapaResultado[sensores.posF - 2][sensores.posC + 2] = sensores.superficie[6];
      mapaCotas[sensores.posF - 2][sensores.posC + 2] = sensores.cota[6];
      mapaResultado[sensores.posF - 1][sensores.posC + 2] = sensores.superficie[7];
      mapaCotas[sensores.posF - 1][sensores.posC + 2] = sensores.cota[7];
      mapaResultado[sensores.posF][sensores.posC + 2] = sensores.superficie[8];
      mapaCotas[sensores.posF][sensores.posC + 2] = sensores.cota[8];
      mapaResultado[sensores.posF - 3][sensores.posC] = sensores.superficie[9];
      mapaCotas[sensores.posF - 3][sensores.posC] = sensores.cota[9];
      mapaResultado[sensores.posF - 3][sensores.posC + 1] = sensores.superficie[10];
      mapaCotas[sensores.posF - 3][sensores.posC + 1] = sensores.cota[10];
      mapaResultado[sensores.posF - 3][sensores.posC + 2] = sensores.superficie[11];
      mapaCotas[sensores.posF - 3][sensores.posC + 2] = sensores.cota[11];
      mapaResultado[sensores.posF - 3][sensores.posC + 3] = sensores.superficie[12];
      mapaCotas[sensores.posF - 3][sensores.posC + 3] = sensores.cota[12];
      mapaResultado[sensores.posF - 2][sensores.posC + 3] = sensores.superficie[13];
      mapaCotas[sensores.posF - 2][sensores.posC + 3] = sensores.cota[13];
      mapaResultado[sensores.posF - 1][sensores.posC + 3] = sensores.superficie[14];
      mapaCotas[sensores.posF - 1][sensores.posC + 3] = sensores.cota[14];
      mapaResultado[sensores.posF][sensores.posC + 3] = sensores.superficie[15];
      mapaCotas[sensores.posF][sensores.posC + 3] = sensores.cota[15];
      break;
    case este:
      for (int j = 1; j < 4; j++)
        for (int i = -j; i <= j; i++) {
          mapaResultado[sensores.posF + i][sensores.posC + j] = sensores.superficie[pos];
          mapaCotas[sensores.posF + i][sensores.posC + j] = sensores.cota[pos++];
        }
      break;
    case sureste:
      mapaResultado[sensores.posF][sensores.posC + 1] = sensores.superficie[1];
      mapaCotas[sensores.posF][sensores.posC + 1] = sensores.cota[1];
      mapaResultado[sensores.posF + 1][sensores.posC + 1] = sensores.superficie[2];
      mapaCotas[sensores.posF + 1][sensores.posC + 1] = sensores.cota[2];
      mapaResultado[sensores.posF + 1][sensores.posC] = sensores.superficie[3];
      mapaCotas[sensores.posF + 1][sensores.posC] = sensores.cota[3];
      mapaResultado[sensores.posF][sensores.posC + 2] = sensores.superficie[4];
      mapaCotas[sensores.posF][sensores.posC + 2] = sensores.cota[4];
      mapaResultado[sensores.posF + 1][sensores.posC + 2] = sensores.superficie[5];
      mapaCotas[sensores.posF + 1][sensores.posC + 2] = sensores.cota[5];
      mapaResultado[sensores.posF + 2][sensores.posC + 2] = sensores.superficie[6];
      mapaCotas[sensores.posF + 2][sensores.posC + 2] = sensores.cota[6];
      mapaResultado[sensores.posF + 2][sensores.posC + 1] = sensores.superficie[7];
      mapaCotas[sensores.posF + 2][sensores.posC + 1] = sensores.cota[7];
      mapaResultado[sensores.posF + 2][sensores.posC] = sensores.superficie[8];
      mapaCotas[sensores.posF + 2][sensores.posC] = sensores.cota[8];
      mapaResultado[sensores.posF][sensores.posC + 3] = sensores.superficie[9];
      mapaCotas[sensores.posF][sensores.posC + 3] = sensores.cota[9];
      mapaResultado[sensores.posF + 1][sensores.posC + 3] = sensores.superficie[10];
      mapaCotas[sensores.posF + 1][sensores.posC + 3] = sensores.cota[10];
      mapaResultado[sensores.posF + 2][sensores.posC + 3] = sensores.superficie[11];
      mapaCotas[sensores.posF + 2][sensores.posC + 3] = sensores.cota[11];
      mapaResultado[sensores.posF + 3][sensores.posC + 3] = sensores.superficie[12];
      mapaCotas[sensores.posF + 3][sensores.posC + 3] = sensores.cota[12];
      mapaResultado[sensores.posF + 3][sensores.posC + 2] = sensores.superficie[13];
      mapaCotas[sensores.posF + 3][sensores.posC + 2] = sensores.cota[13];
      mapaResultado[sensores.posF + 3][sensores.posC + 1] = sensores.superficie[14];
      mapaCotas[sensores.posF + 3][sensores.posC + 1] = sensores.cota[14];
      mapaResultado[sensores.posF + 3][sensores.posC] = sensores.superficie[15];
      mapaCotas[sensores.posF + 3][sensores.posC] = sensores.cota[15];
      break;
    case sur:
      for (int j = 1; j < 4; j++)
        for (int i = -j; i <= j; i++) {
          mapaResultado[sensores.posF + j][sensores.posC - i] = sensores.superficie[pos];
          mapaCotas[sensores.posF + j][sensores.posC - i] = sensores.cota[pos++];
        }
      break;
    case suroeste:
      mapaResultado[sensores.posF + 1][sensores.posC] = sensores.superficie[1];
      mapaCotas[sensores.posF + 1][sensores.posC] = sensores.cota[1];
      mapaResultado[sensores.posF + 1][sensores.posC - 1] = sensores.superficie[2];
      mapaCotas[sensores.posF + 1][sensores.posC - 1] = sensores.cota[2];
      mapaResultado[sensores.posF][sensores.posC - 1] = sensores.superficie[3];
      mapaCotas[sensores.posF][sensores.posC - 1] = sensores.cota[3];
      mapaResultado[sensores.posF + 2][sensores.posC] = sensores.superficie[4];
      mapaCotas[sensores.posF + 2][sensores.posC] = sensores.cota[4];
      mapaResultado[sensores.posF + 2][sensores.posC - 1] = sensores.superficie[5];
      mapaCotas[sensores.posF + 2][sensores.posC - 1] = sensores.cota[5];
      mapaResultado[sensores.posF + 2][sensores.posC - 2] = sensores.superficie[6];
      mapaCotas[sensores.posF + 2][sensores.posC - 2] = sensores.cota[6];
      mapaResultado[sensores.posF + 1][sensores.posC - 2] = sensores.superficie[7];
      mapaCotas[sensores.posF + 1][sensores.posC - 2] = sensores.cota[7];
      mapaResultado[sensores.posF][sensores.posC - 2] = sensores.superficie[8];
      mapaCotas[sensores.posF][sensores.posC - 2] = sensores.cota[8];
      mapaResultado[sensores.posF + 3][sensores.posC] = sensores.superficie[9];
      mapaCotas[sensores.posF + 3][sensores.posC] = sensores.cota[9];
      mapaResultado[sensores.posF + 3][sensores.posC - 1] = sensores.superficie[10];
      mapaCotas[sensores.posF + 3][sensores.posC - 1] = sensores.cota[10];
      mapaResultado[sensores.posF + 3][sensores.posC - 2] = sensores.superficie[11];
      mapaCotas[sensores.posF + 3][sensores.posC - 2] = sensores.cota[11];
      mapaResultado[sensores.posF + 3][sensores.posC - 3] = sensores.superficie[12];
      mapaCotas[sensores.posF + 3][sensores.posC - 3] = sensores.cota[12];
      mapaResultado[sensores.posF + 2][sensores.posC - 3] = sensores.superficie[13];
      mapaCotas[sensores.posF + 2][sensores.posC - 3] = sensores.cota[13];
      mapaResultado[sensores.posF + 1][sensores.posC - 3] = sensores.superficie[14];
      mapaCotas[sensores.posF + 1][sensores.posC - 3] = sensores.cota[14];
      mapaResultado[sensores.posF][sensores.posC - 3] = sensores.superficie[15];
      mapaCotas[sensores.posF][sensores.posC - 3] = sensores.cota[15];
      break;
    case oeste:
      for (int j = 1; j < 4; j++)
        for (int i = -j; i <= j; i++) {
          mapaResultado[sensores.posF - i][sensores.posC - j] = sensores.superficie[pos];
          mapaCotas[sensores.posF - i][sensores.posC - j] = sensores.cota[pos++];
        }
      break;
    case noroeste:
      mapaResultado[sensores.posF][sensores.posC - 1] = sensores.superficie[1];
      mapaCotas[sensores.posF][sensores.posC - 1] = sensores.cota[1];
      mapaResultado[sensores.posF - 1][sensores.posC - 1] = sensores.superficie[2];
      mapaCotas[sensores.posF - 1][sensores.posC - 1] = sensores.cota[2];
      mapaResultado[sensores.posF - 1][sensores.posC] = sensores.superficie[3];
      mapaCotas[sensores.posF - 1][sensores.posC] = sensores.cota[3];
      mapaResultado[sensores.posF][sensores.posC - 2] = sensores.superficie[4];
      mapaCotas[sensores.posF][sensores.posC - 2] = sensores.cota[4];
      mapaResultado[sensores.posF - 1][sensores.posC - 2] = sensores.superficie[5];
      mapaCotas[sensores.posF - 1][sensores.posC - 2] = sensores.cota[5];
      mapaResultado[sensores.posF - 2][sensores.posC - 2] = sensores.superficie[6];
      mapaCotas[sensores.posF - 2][sensores.posC - 2] = sensores.cota[6];
      mapaResultado[sensores.posF - 2][sensores.posC - 1] = sensores.superficie[7];
      mapaCotas[sensores.posF - 2][sensores.posC - 1] = sensores.cota[7];
      mapaResultado[sensores.posF - 2][sensores.posC] = sensores.superficie[8];
      mapaCotas[sensores.posF - 2][sensores.posC] = sensores.cota[8];
      mapaResultado[sensores.posF][sensores.posC - 3] = sensores.superficie[9];
      mapaCotas[sensores.posF][sensores.posC - 3] = sensores.cota[9];
      mapaResultado[sensores.posF - 1][sensores.posC - 3] = sensores.superficie[10];
      mapaCotas[sensores.posF - 1][sensores.posC - 3] = sensores.cota[10];
      mapaResultado[sensores.posF - 2][sensores.posC - 3] = sensores.superficie[11];
      mapaCotas[sensores.posF - 2][sensores.posC - 3] = sensores.cota[11];
      mapaResultado[sensores.posF - 3][sensores.posC - 3] = sensores.superficie[12];
      mapaCotas[sensores.posF - 3][sensores.posC - 3] = sensores.cota[12];
      mapaResultado[sensores.posF - 3][sensores.posC - 2] = sensores.superficie[13];
      mapaCotas[sensores.posF - 3][sensores.posC - 2] = sensores.cota[13];
      mapaResultado[sensores.posF - 3][sensores.posC - 1] = sensores.superficie[14];
      mapaCotas[sensores.posF - 3][sensores.posC - 1] = sensores.cota[14];
      mapaResultado[sensores.posF - 3][sensores.posC] = sensores.superficie[15];
      mapaCotas[sensores.posF - 3][sensores.posC] = sensores.cota[15];
      break;
  }
}



/**
 * @brief Determina si una casilla es transitable para el técnico.
 * En esta práctica, si el técnico tiene zapatillas, el bosque ('B') es transitable.
 * @param f Fila de la casilla.
 * @param c Columna de la casilla.
 * @param tieneZapatillas Indica si el agente posee las zapatillas.
 * @return true si la casilla es transitable.
 */
bool ComportamientoTecnico::EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas) {
  if (f < 0 || f >= mapaResultado.size() || c < 0 || c >= mapaResultado[0].size()) return false;
  return es_camino(mapaResultado[f][c]);  // Solo 'C', 'S', 'D', 'U' son transitables en Nivel 0
}

/**
 * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
 * Para el técnico: desnivel máximo siempre 1.
 * @param actual Estado actual del agente (fila, columna, orientacion).
 * @return true si el desnivel con la casilla de delante es admisible.
 */
bool ComportamientoTecnico::EsAccesiblePorAltura(const ubicacion &actual) {
  ubicacion del = Delante(actual);
  if (del.f < 0 || del.f >= mapaCotas.size() || del.c < 0 || del.c >= mapaCotas[0].size()) return false;
  int desnivel = abs(mapaCotas[del.f][del.c] - mapaCotas[actual.f][actual.c]);
  if (desnivel > 1) return false;
  return true;
}

/**
 * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
 * Calcula la casilla frontal según la orientación actual (8 direcciones).
 * @param actual Estado actual del agente (fila, columna, orientacion).
 * @return Estado con la fila y columna de la casilla de enfrente.
 */
ubicacion ComportamientoTecnico::Delante(const ubicacion &actual) const {
  ubicacion delante = actual;
  switch (actual.brujula) {
    case 0: delante.f--; break;                        // norte
    case 1: delante.f--; delante.c++; break;     // noreste
    case 2: delante.c++; break;                     // este
    case 3: delante.f++; delante.c++; break;     // sureste
    case 4: delante.f++; break;                        // sur
    case 5: delante.f++; delante.c--; break;     // suroeste
    case 6: delante.c--; break;                     // oeste
    case 7: delante.f--; delante.c--; break;     // noroeste
  }
  return delante;
}

ubicacion ComportamientoTecnico::Izquierda(const ubicacion &actual) const
{
  ubicacion izquierda = actual;
  switch (actual.brujula)
  {
  case 0:
    izquierda.f--;
    izquierda.c--;
    break; // norte -> noroeste
  case 1:
    izquierda.f--;
    break; // noreste -> norte
  case 2:
    izquierda.f--;
    izquierda.c++;
    break; // este -> noreste
  case 3:
    izquierda.c++;
    break; // sureste -> este
  case 4:
    izquierda.f++;
    izquierda.c++;
    break; // sur -> sureste
  case 5:
    izquierda.f++;
    break; // suroeste -> sur
  case 6:
    izquierda.f++;
    izquierda.c--;
    break; // oeste -> suroeste
  case 7:
    izquierda.c--;
    break; // noroeste -> oeste
  }
  return izquierda;
}

ubicacion ComportamientoTecnico::Derecha(const ubicacion &actual) const
{
  ubicacion derecha = actual;
  switch (actual.brujula)
  {
  case 0:
    derecha.f--;
    derecha.c++;
    break; // norte -> noreste
  case 1:
    derecha.c++;
    break; // noreste -> este
  case 2:
    derecha.f++;
    derecha.c++;
    break; // este -> sureste
  case 3:
    derecha.f++;
    break; // sureste -> sur
  case 4:
    derecha.f++;
    derecha.c--;
    break; // sur -> suroeste
  case 5:
    derecha.c--;
    break; // suroeste -> oeste
  case 6:
    derecha.f--;
    derecha.c--;
    break; // oeste -> noroeste
  case 7:
    derecha.f--;
    break; // noroeste -> norte
  }
  return derecha;
}

/**
 * @brief Imprime por consola la secuencia de acciones de un plan.
 *
 * @param plan  Lista de acciones del plan.
 */
void ComportamientoTecnico::PintaPlan(const list<Action> &plan)
{
  auto it = plan.begin();
  while (it != plan.end())
  {
    if (*it == WALK)
    {
      cout << "W ";
    }
    else if (*it == JUMP)
    {
      cout << "J ";
    }
    else if (*it == TURN_SR)
    {
      cout << "r ";
    }
    else if (*it == TURN_SL)
    {
      cout << "l ";
    }
    else if (*it == COME)
    {
      cout << "C ";
    }
    else if (*it == IDLE)
    {
      cout << "I ";
    }
    else
    {
      cout << "-_ ";
    }
    it++;
  }
  cout << "( longitud " << plan.size() << ")" << endl;
}



/**
 * @brief Convierte un plan de acciones en una lista de casillas para
 *        su visualización en el mapa 2D.
 *
 * @param st    Estado de partida.
 * @param plan  Lista de acciones del plan.
 */
void ComportamientoTecnico::VisualizaPlan(const ubicacion &st,
                                            const list<Action> &plan)
{
  listaPlanCasillas.clear();
  ubicacion cst = st;

  listaPlanCasillas.push_back({cst.f, cst.c, WALK});
  auto it = plan.begin();
  while (it != plan.end())
  {

    switch (*it)
    {
    case JUMP:
      switch (cst.brujula)
      {
      case 0:
        cst.f--;
        break;
      case 1:
        cst.f--;
        cst.c++;
        break;
      case 2:
        cst.c++;
        break;
      case 3:
        cst.f++;
        cst.c++;
        break;
      case 4:
        cst.f++;
        break;
      case 5:
        cst.f++;
        cst.c--;
        break;
      case 6:
        cst.c--;
        break;
      case 7:
        cst.f--;
        cst.c--;
        break;
      }
      if (cst.f >= 0 && cst.f < mapaResultado.size() &&
          cst.c >= 0 && cst.c < mapaResultado[0].size())
        listaPlanCasillas.push_back({cst.f, cst.c, JUMP});
    case WALK:
      switch (cst.brujula)
      {
      case 0:
        cst.f--;
        break;
      case 1:
        cst.f--;
        cst.c++;
        break;
      case 2:
        cst.c++;
        break;
      case 3:
        cst.f++;
        cst.c++;
        break;
      case 4:
        cst.f++;
        break;
      case 5:
        cst.f++;
        cst.c--;
        break;
      case 6:
        cst.c--;
        break;
      case 7:
        cst.f--;
        cst.c--;
        break;
      }
      if (cst.f >= 0 && cst.f < mapaResultado.size() &&
          cst.c >= 0 && cst.c < mapaResultado[0].size())
        listaPlanCasillas.push_back({cst.f, cst.c, WALK});
      break;
    case TURN_SR:
      cst.brujula = (Orientacion) (( (int) cst.brujula + 1) % 8);
      break;
    case TURN_SL:
      cst.brujula = (Orientacion) (( (int) cst.brujula + 7) % 8);
      break;
    }
    it++;
  }
}
