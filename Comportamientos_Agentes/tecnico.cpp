#include "tecnico.hpp"
#include "motorlib/util.h"
#include <iostream>
#include <queue>
#include <set>
#include <map>

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
      accion = ComportamientoTecnicoNivel_3(sensores); break;
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
  else if (vi <= vd) return 1;
  else return 3;
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
  Action accion = IDLE;

  if (pasos_evasion > 0) {
    pasos_evasion--;

    if (sensores.agentes[2] != 'i' &&
      sensores.superficie[2] != 'P' &&
      sensores.superficie[2] != 'M' &&
      (sensores.superficie[2] != 'B' ||
        (sensores.superficie[2] != 'B' && tiene_zapatillas))) {
      ubicacion actual;
      actual.f = sensores.posF;
      actual.c = sensores.posC;
      actual.brujula = sensores.rumbo;

      if (EsAccesiblePorAltura(actual)) {
        return last_action;
      }
    }

    return TURN_SR;
  }

  if (comprobando_obstaculiza) {
    if (sensores.agentes[2] == 'i' || sensores.agentes[6] == 'i') {
      accion = TURN_SL;
      pasos_evasion = 2;
    } else if (sensores.agentes[1] == 'i' || sensores.agentes[4] == 'i' || sensores.agentes[5] == 'i') {
      accion = TURN_SR;
      pasos_evasion = 2;
    } else if (sensores.agentes[3] == 'i' || sensores.agentes[7] == 'i' || sensores.agentes[8] == 'i') {
      accion = TURN_SL;
      pasos_evasion = 2;
    }
  }

  comprobando_obstaculiza = (comprobando_obstaculiza+1)%2;
  last_action = accion;

  return accion;
}

// ─────────────────────────────────────────────────────────────────────────────
// COMIENZO NIVEL 3 TECNICO
// ─────────────────────────────────────────────────────────────────────────────

int ComportamientoTecnico::heuristica(const EstadoT &st, const EstadoT &fin) {
    return max(abs(st.site.f - fin.site.f), abs(st.site.c - fin.site.c));
}

int ComportamientoTecnico::costeTerreno(unsigned char t, bool zapatillas, int altura_origen, int altura_destino) {
  if (altura_origen == -1 || altura_destino == -1) {
    switch (t) {
      case 'A': return 5;
      case 'H': return 2;
      default:  return 1;
    }
  }

  int base = 0;
  switch (t) {
    case 'H': base = 6; break;
    case 'A': base = 60; break;
    case 'S': base = 3; break;
    default: base = 1; break;
  }

  int diff = altura_destino - altura_origen;
  if (diff > 0) base += 5;
  else if (diff < 0) base -= 2;

  return max(base, 1);
}

EstadoT ComportamientoTecnico::NextCasillaTecnico(const EstadoT &st) {
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

bool ComportamientoTecnico::CasillaAccesibleTecnico(const EstadoT &st,
                              const vector<vector<unsigned char>> &terreno,
                              const vector<vector<unsigned char>> &altura) {
  EstadoT next = NextCasillaTecnico(st);
  bool transitable = terreno[next.site.f][next.site.c] != 'P' and
                terreno[next.site.f][next.site.c] != 'M' and
                (terreno[next.site.f][next.site.c] != 'B' or
                (terreno[next.site.f][next.site.c] == 'B' and st.zapatillas));
  bool alturaOK = abs((int)altura[next.site.f][next.site.c] -
                    (int)altura[st.site.f][st.site.c]) <= 1;
  return transitable and alturaOK;
}

EstadoT ComportamientoTecnico::applyT(Action accion, const EstadoT &st,
               const vector<vector<unsigned char>> &terreno,
               const vector<vector<unsigned char>> &altura) {
  EstadoT next = st;
  switch (accion) {
    case WALK:
      if (CasillaAccesibleTecnico(st, terreno, altura)) {
        next = NextCasillaTecnico(st);
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

list<Action> ComportamientoTecnico::A_Star(const EstadoT &inicio, const EstadoT &final,
                                           const vector<vector<unsigned char>> &terreno,
                                           const vector<vector<unsigned char>> &altura) {

  priority_queue<NodoT, vector<NodoT>, Comparador> frontier;
  map<EstadoT, int> coste_minimo;
  list<Action> path;

  int coste_extra = 0,
      nuevo_coste = 0;

  NodoT current_node;
  current_node.estado = inicio;
  current_node.g = 0;
  current_node.h = heuristica(inicio, final);

  frontier.push(current_node);
  coste_minimo[inicio] = 0;

  bool SolutionFound = false;

  while (!frontier.empty() && !SolutionFound) {
    current_node = frontier.top();
    frontier.pop();

    if (current_node.g > coste_minimo[current_node.estado])
      continue;

    if (current_node.estado.site.f == final.site.f &&
      current_node.estado.site.c == final.site.c) {
      SolutionFound = true;
      break;
    }

    if (terreno[current_node.estado.site.f][current_node.estado.site.c] == 'D') {
      current_node.estado.zapatillas = true;
    }

    NodoT child_Walk = current_node;
    child_Walk.estado = applyT(WALK, current_node.estado, terreno, altura);

    if (!(child_Walk.estado == current_node.estado)) {
      coste_extra = costeTerreno(
        terreno[current_node.estado.site.f][current_node.estado.site.c],
        current_node.estado.zapatillas,
        altura[current_node.estado.site.f][current_node.estado.site.c],
        altura[child_Walk.estado.site.f][child_Walk.estado.site.c]
      );

      nuevo_coste = current_node.g + coste_extra;

      if (coste_minimo.find(child_Walk.estado) == coste_minimo.end() ||
        nuevo_coste < coste_minimo[child_Walk.estado]) {

        child_Walk.g = nuevo_coste;
        child_Walk.h = heuristica(child_Walk.estado, final);

        coste_minimo[child_Walk.estado] = nuevo_coste;

        child_Walk.secuencia.push_back(WALK);
        frontier.push(child_Walk);
      }
    }

    NodoT child_Turn_SR = current_node;
    child_Turn_SR.estado = applyT(TURN_SR, current_node.estado, terreno, altura);

    if (!(child_Turn_SR.estado == current_node.estado)) {
      coste_extra = costeTerreno(
        terreno[current_node.estado.site.f][current_node.estado.site.c],
        current_node.estado.zapatillas
      );

      nuevo_coste = current_node.g + coste_extra;

      if (coste_minimo.find(child_Turn_SR.estado) == coste_minimo.end() ||
        nuevo_coste < coste_minimo[child_Turn_SR.estado]) {

        child_Turn_SR.g = nuevo_coste;
        child_Turn_SR.h = heuristica(child_Turn_SR.estado, final);

        coste_minimo[child_Turn_SR.estado] = nuevo_coste;

        child_Turn_SR.secuencia.push_back(TURN_SR);
        frontier.push(child_Turn_SR);
      }
    }

    NodoT child_Turn_SL = current_node;
    child_Turn_SL.estado = applyT(TURN_SL, current_node.estado, terreno, altura);

    if (!(child_Turn_SL.estado == current_node.estado)) {
      coste_extra = costeTerreno(
        terreno[current_node.estado.site.f][current_node.estado.site.c],
        current_node.estado.zapatillas
      );

      nuevo_coste = current_node.g + coste_extra;

      if (coste_minimo.find(child_Turn_SL.estado) == coste_minimo.end() ||
        nuevo_coste < coste_minimo[child_Turn_SL.estado]) {

        child_Turn_SL.g = nuevo_coste;
        child_Turn_SL.h = heuristica(child_Turn_SL.estado, final);

        coste_minimo[child_Turn_SL.estado] = nuevo_coste;

        child_Turn_SL.secuencia.push_back(TURN_SL);
        frontier.push(child_Turn_SL);
      }
    }
  }

  if (SolutionFound)
    path = current_node.secuencia;

  return path;
}

/**
 * @brief Comportamiento del técnico para el Nivel 3.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_3(Sensores sensores) {
  Action accion = IDLE;

  if (!hayPlan) {
    EstadoT inicio, fin;
    inicio.site.f = sensores.posF;
    inicio.site.c = sensores.posC;
    inicio.site.brujula = sensores.rumbo;
    inicio.zapatillas = tiene_zapatillas;
    fin.site.f = sensores.BelPosF;
    fin.site.c = sensores.BelPosC;
    plan = A_Star(inicio, fin, mapaResultado, mapaCotas);
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
 * @brief Comportamiento del técnico para el Nivel 4.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_4(Sensores sensores) {
  return IDLE;
}

int ComportamientoTecnico::CosteInstallEnergia(unsigned char t) {
    switch (t) {
        case 'A': return 60;
        case 'H': return 45;
        case 'S': return 25;
        case 'C': return 15;
        case 'U': return 15;
        default: return 30;
    }
}

int ComportamientoTecnico::CosteInstallEco(unsigned char t) {
    switch (t) {
        case 'A': return 50;
        case 'H': return 45;
        case 'S': return 25;
        case 'C': return 15;
        case 'U': return 15;
        default: return 30;
    }
}

int ComportamientoTecnico::CosteRaise(unsigned char t) {
    switch (t) {
        case 'H': return 55;
        case 'S': return 30;
        case 'C': return 10;
        case 'U': return 10;
        default: return 40;
    }
}

int ComportamientoTecnico::CosteDig(unsigned char t) {
    switch (t) {
        case 'H': return 65;
        case 'S': return 40;
        case 'C': return 25;
        case 'U': return 25;
        default: return 50;
    }
}

list<Paso> ComportamientoTecnico::PlanificarTuberias(int belF, int belC, int max_energia, int max_eco) {
    const int df[] = {-1, 1,  0, 0};
    const int dc[] = { 0, 0,  1,-1};

    int filas    = mapaResultado.size();
    int columnas = mapaResultado[0].size();

    list<NodoBFS4T> frontier;

    map<EstadoTuberiaT, int> min_energia_llegada;
    map<EstadoTuberiaT, int> min_eco_llegada;

    // --- 1. CONFIGURACIÓN INICIAL ---
    unsigned char tIni = mapaResultado[belF][belC];
    int cIni = (int)mapaCotas[belF][belC];

    vector<int> opsIni = {0};
    if (tIni != 'A' && cIni < 9) opsIni.push_back(1);
    if (tIni != 'A' && cIni > 1) opsIni.push_back(-1);

    for (int op : opsIni) {
        int energia_inicial = 0;
        int eco_inicial = 0;

        // El primer paso (origen) no cuesta INSTALL, solo cuesta si lo excavamos/subimos
        if (op == 1) {
            energia_inicial += CosteRaise(tIni);
            eco_inicial += CosteRaise(tIni);
        } else if (op == -1) {
            energia_inicial += CosteDig(tIni);
            eco_inicial += CosteDig(tIni);
        }

        if (energia_inicial <= max_energia && eco_inicial <= max_eco) {
            NodoBFS4T nodo;
            nodo.estado = {belF, belC, op};
            nodo.energia_gastada = energia_inicial;
            nodo.eco_gastado = eco_inicial;

            // Guardamos el coste individual en el Paso
            Paso p; p.fil = belF; p.col = belC; p.op = op;
            p.energia_paso = energia_inicial;
            p.eco_paso = eco_inicial;
            nodo.camino.push_back(p);

            frontier.push_back(nodo);
            min_energia_llegada[nodo.estado] = energia_inicial;
            min_eco_llegada[nodo.estado] = eco_inicial;
        }
    }

    // --- 2. BUCLE BFS PRINCIPAL ---
    while (!frontier.empty()) {

        NodoBFS4T current = frontier.front();
        frontier.pop_front();

        int f  = current.estado.f;
        int c  = current.estado.c;
        int op = current.estado.op;
        int cotaActual = (int)mapaCotas[f][c] + op;

        // --- META ALCANZADA (BLOQUE DE LOGS AMPLIADO) ---
        if (mapaResultado[f][c] == 'U') {
          return current.camino;
        }

        // Exploración de vecinos
        for (int d = 0; d < 4; d++) {
            int nf = f + df[d];
            int nc = c + dc[d];

            if (nf < 0 || nf >= filas || nc < 0 || nc >= columnas) continue;

            unsigned char tNext = mapaResultado[nf][nc];
            if (tNext == 'P' || tNext == 'M' || tNext == 'B') continue;

            int cotaNextOrig = (int)mapaCotas[nf][nc];

            vector<int> opsNext = {0};
            if (tNext != 'A' && cotaNextOrig < 9) opsNext.push_back(1);
            if (tNext != 'A' && cotaNextOrig > 1) opsNext.push_back(-1);

            for (int onext : opsNext) {
                int cotaNextFinal = cotaNextOrig + onext;
                int diff = cotaActual - cotaNextFinal;

                if (diff == 0 || diff == 1) { // Cumple la gravedad

                    // Calculamos el coste de entrar en esta casilla (INSTALL) y modificarla
                    // Sacamos el terreno de la casilla origen de este tramo
                    unsigned char tActual = mapaResultado[f][c];

                    // El coste de conectar es la suma del terreno de AMBOS agentes
                    int e_paso_actual = CosteInstallEnergia(tActual) + CosteInstallEnergia(tNext);
                    int eco_paso_actual = CosteInstallEco(tActual) + CosteInstallEco(tNext);

                    // Si además el Ingeniero modifica la casilla de destino, sumamos ese coste
                    if (onext == 1) {
                        e_paso_actual += CosteRaise(tNext);
                        eco_paso_actual += CosteRaise(tNext);
                    } else if (onext == -1) {
                        e_paso_actual += CosteDig(tNext);
                        eco_paso_actual += CosteDig(tNext);
                    }

                    int nueva_energia = current.energia_gastada + e_paso_actual;
                    int nuevo_eco = current.eco_gastado + eco_paso_actual;

                    if (nueva_energia > max_energia || nuevo_eco > max_eco) {
                        continue;
                    }

                    EstadoTuberiaT nextState = {nf, nc, onext};

                    bool mejor_energia = min_energia_llegada.find(nextState) == min_energia_llegada.end() || nueva_energia < min_energia_llegada[nextState];
                    bool mejor_eco = min_eco_llegada.find(nextState) == min_eco_llegada.end() || nuevo_eco < min_eco_llegada[nextState];

                    if (mejor_energia || mejor_eco) {
                        if (mejor_energia) min_energia_llegada[nextState] = nueva_energia;
                        if (mejor_eco) min_eco_llegada[nextState] = nuevo_eco;

                        NodoBFS4T child = current;
                        child.estado = nextState;
                        child.energia_gastada = nueva_energia;
                        child.eco_gastado = nuevo_eco;

                        // Guardamos el coste individual en el Paso
                        Paso pNext; pNext.fil = nf; pNext.col = nc; pNext.op = onext;
                        pNext.energia_paso = e_paso_actual;
                        pNext.eco_paso = eco_paso_actual;
                        child.camino.push_back(pNext);

                        frontier.push_back(child);
                    }
                }
            }
        }
    }

    cout << "No se encontro ningun plan valido que respete los limites." << endl;
    return {};
}

/**
 * @brief Comportamiento del técnico para el Nivel 5.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoTecnico::ComportamientoTecnicoNivel_5(Sensores sensores) {

    // Zapatillas para pisar el bosque
    if (sensores.superficie[0] == 'D') {
        tiene_zapatillas = true;
    }

    Action accion = IDLE;

    switch(estadoTecnico) {

        case 0: // FASE 0: PLANIFICANDO LA RED
            if (!plan_calculado) {
                // El técnico calcula el mismo plan que el ingeniero
                planTuberiasTecnico = PlanificarTuberias(sensores.BelPosF, sensores.BelPosC, sensores.energia, sensores.max_ecologico);
                plan_calculado = true;
                tramo_actual = 0;
            }
            if (planTuberiasTecnico.size() > 1) {
                estadoTecnico = 1; // Pasamos a esperar órdenes
            }
        break;

        case 1: // FASE 1: ESPERANDO LA SEÑAL (COME)
            // El ingeniero nos avisa de que el terreno está listo
            if (sensores.venpaca) {
                estadoTecnico = 2; // Toca moverse
                hayPlan = false;   // Reseteamos nuestro A* por seguridad
            }
        break;

        case 2: { // FASE 2: DESPLAZÁNDOSE AL DESTINO DEL TRAMO
            auto it = planTuberiasTecnico.begin();
            // ¡OJO AQUÍ! El Ingeniero está en el origen del tramo. El Técnico va a la casilla DESTINO (+1).
            std::advance(it, tramo_actual + 1);
            int destF = it->fil;
            int destC = it->col;

            // ¿Hemos llegado a nuestra posición?
            if (sensores.posF == destF && sensores.posC == destC) {
                estadoTecnico = 3; // Siguiente fase: Girarse
            } else {
                // Usamos el A* del Nivel 3 para llegar
                if (!hayPlan) {
                    EstadoT inicio, fin;
                    inicio.site.f = sensores.posF;
                    inicio.site.c = sensores.posC;
                    inicio.site.brujula = sensores.rumbo;
                    inicio.zapatillas = tiene_zapatillas;
                    fin.site.f = destF;
                    fin.site.c = destC;

                    plan = A_Star(inicio, fin, mapaResultado, mapaCotas);
                    hayPlan = plan.size() > 0;
                }

                if (hayPlan && !plan.empty()) {
                    accion = plan.front();
                    plan.pop_front();
                    if (plan.empty()) hayPlan = false;
                }
            }
        break;
        }

        case 3: { // FASE 3: ENCARAR AL INGENIERO
            auto it = planTuberiasTecnico.begin();
            // Leemos dónde se supone que está parado el Ingeniero
            std::advance(it, tramo_actual);
            int ingF = it->fil;
            int ingC = it->col;

            // 1. Calculamos hacia dónde debemos mirar
            Orientacion target = norte;
            if (ingF < sensores.posF) target = norte;
            else if (ingF > sensores.posF) target = sur;
            else if (ingC > sensores.posC) target = este;
            else if (ingC < sensores.posC) target = oeste;

            // 2. Si no miramos hacia el ingeniero, giramos
            if (sensores.rumbo != target) {
                int diff = (target - sensores.rumbo + 8) % 8;
                if (diff <= 4) accion = TURN_SR;
                else accion = TURN_SL;
            } else {
                // 3. SINCRONIZACIÓN PERFECTA:
                // Ya estamos mirando al Ingeniero. Vamos a quedarnos quietos (IDLE) 1 tick
                // para que el Ingeniero actualice su sensor 'enfrente' y vea que estamos listos.
                estadoTecnico = 4;
                accion = IDLE;
            }
        break;
        }

        case 4: // FASE 4: INSTALACIÓN SIMULTÁNEA
          if (sensores.enfrente) {
            accion = INSTALL; // Ambos ejecutan esto en el mismo tick
            tramo_actual++;   // Avanzamos al siguiente tramo

            if (tramo_actual >= planTuberiasTecnico.size() - 1) {
                estadoTecnico = 5; // ¡Obra terminada!
            } else {
                estadoTecnico = 1; // Volvemos a esperar el COME del siguiente tramo
            }
          }
        break;

        case 5: // FASE 5: FIN DEL JUEGO
            accion = IDLE;
        break;
    }

    return accion;
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
