#include "ingeniero.hpp"
#include "motorlib/util.h"
#include <iostream>
#include <queue>
#include <set>

using namespace std;

// =========================================================================
// ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
// =========================================================================

Action ComportamientoIngeniero::think(Sensores sensores)
{
  Action accion = IDLE;

  switch (sensores.nivel) {
    case 0:
      accion = ComportamientoIngenieroNivel_0(sensores); break;
    case 1:
      accion = ComportamientoIngenieroNivel_1(sensores); break;
    case 2:
      accion = ComportamientoIngenieroNivel_2(sensores); break;
    case 3:
      accion = ComportamientoIngenieroNivel_3(sensores); break;
    case 4:
      accion = ComportamientoIngenieroNivel_4(sensores); break;
    case 5:
      accion = ComportamientoIngenieroNivel_5(sensores); break;
    case 6:
      accion = ComportamientoIngenieroNivel_6(sensores); break;
  }

  return accion;
}

// ─────────────────────────────────────────────────────────────────────────────
// COMIENZO NIVEL 0
// ─────────────────────────────────────────────────────────────────────────────

int VeoCasillaInteresanteI(char i, char c, char d, bool zap, int vis_i, int vis_c, int vis_d)
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

char ViablePorAlturaI (char casilla, int dif, bool zap)
{
  if (abs(dif) <= 1 or (zap and abs(dif) <= 2)) return casilla;
  else return 'P';
}


// Niveles iniciales (Comportamientos reactivos simples)
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_0(Sensores sensores)
{
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

  char i = ViablePorAlturaI(sensores.superficie[1], sensores.cota[1]-sensores.cota[0], tiene_zapatillas);
  char c = ViablePorAlturaI(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], tiene_zapatillas);
  char d = ViablePorAlturaI(sensores.superficie[3], sensores.cota[3]-sensores.cota[0], tiene_zapatillas);

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

  if (sensores.agentes[2] == 't' || c == 'P') { c = 'P'; vis_c = INT_MAX; }
  if (sensores.agentes[1] == 't' || i == 'P') { i = 'P'; vis_i = INT_MAX; }
  if (sensores.agentes[3] == 't' || d == 'P') { d = 'P'; vis_d = INT_MAX; }

  int pos = VeoCasillaInteresanteI(i, c, d, tiene_zapatillas, vis_i, vis_c, vis_d);

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

// ─────────────────────────────────────────────────────────────────────────────
// COMIENZO NIVEL 1
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Comprueba si una celda es de tipo camino transitable.
 * @param c Carácter que representa el tipo de superficie.
 * @return true si es camino ('C'), zapatillas ('D') o meta ('U').
 */
bool ComportamientoIngeniero::es_camino(unsigned char c) const
{
  return (c == 'C' || c == 'D' || c == 'U');
}

int VeoCasillaInteresanteI_N1(char i, char c, char d, bool zap, int vis_i, int vis_c, int vis_d)
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
 * @brief Comportamiento reactivo del ingeniero para el Nivel 1.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_1(Sensores sensores)
{
  Action accion = IDLE;
  ActualizarMapa(sensores);

  if (mapa_visitas.empty()) {
    mapa_visitas = vector<vector<int>>(mapaResultado.size(), vector<int>(mapaResultado[0].size(), 0));
  }

  mapa_visitas[sensores.posF][sensores.posC]++;

  if (sensores.superficie[0] == 'D') {
    tiene_zapatillas = true;
  }

  char i = ViablePorAlturaI(sensores.superficie[1], sensores.cota[1]-sensores.cota[0], tiene_zapatillas);
  char c = ViablePorAlturaI(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], tiene_zapatillas);
  char d = ViablePorAlturaI(sensores.superficie[3], sensores.cota[3]-sensores.cota[0], tiene_zapatillas);

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

  if (sensores.agentes[2] == 't' || c == 'P') { c = 'P'; vis_c = INT_MAX; }
  if (sensores.agentes[1] == 't' || i == 'P') { i = 'P'; vis_i = INT_MAX; }
  if (sensores.agentes[3] == 't' || d == 'P') { d = 'P'; vis_d = INT_MAX; }

  int pos = VeoCasillaInteresanteI_N1(i, c, d, tiene_zapatillas, vis_i, vis_c, vis_d);

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

// ─────────────────────────────────────────────────────────────────────────────
// COMIENZO NIVEL 2 INGENIERO
// ─────────────────────────────────────────────────────────────────────────────

EstadoI ComportamientoIngeniero::NextCasillaIngeniero(const EstadoI &st) {
  EstadoI siguiente = st;
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

bool ComportamientoIngeniero::CasillaAccesibleIngeniero(const EstadoI &st,
                              const vector<vector<unsigned char>> &terreno,
                              const vector<vector<unsigned char>> &altura) {
  EstadoI next = NextCasillaIngeniero(st);
  bool check1 = terreno[next.site.f][next.site.c] != 'P' and
                terreno[next.site.f][next.site.c] != 'B' and
                terreno[next.site.f][next.site.c] != 'M';
  bool check2 = abs((int)altura[next.site.f][next.site.c] -
                    (int)altura[st.site.f][st.site.c]) <= 1 or
                (abs((int)altura[next.site.f][next.site.c] -
                    (int)altura[st.site.f][st.site.c]) <= 2 and st.zapatillas);
  return check1 and check2;
}

bool ComportamientoIngeniero::IntermediaAccesible(const EstadoI &st,
                                                  const vector<vector<unsigned char>> &terreno,
                                                  const vector<vector<unsigned char>> &altura) {

    EstadoI mid = NextCasillaIngeniero(st);

    bool transitable = terreno[mid.site.f][mid.site.c] != 'P' &&
                       terreno[mid.site.f][mid.site.c] != 'B' &&
                       terreno[mid.site.f][mid.site.c] != 'M';

    int delta = abs((int)altura[mid.site.f][mid.site.c] -
                    (int)altura[st.site.f][st.site.c]);
    bool alturaOk = delta <= 1 || (delta <= 2 && st.zapatillas);

    return transitable && alturaOk;
}

EstadoI ComportamientoIngeniero::applyI(Action accion, const EstadoI &st,
               const vector<vector<unsigned char>> &terreno,
               const vector<vector<unsigned char>> &altura) {
  EstadoI next = st;
  switch (accion) {
    case WALK:
      if (CasillaAccesibleIngeniero(st, terreno, altura)) {
        next = NextCasillaIngeniero(st);
      }
    break;
    case JUMP:
      if (IntermediaAccesible(st, terreno, altura)) {
        EstadoI mid = NextCasillaIngeniero(st);

        if (CasillaAccesibleIngeniero(mid, terreno, altura))
          next = NextCasillaIngeniero(mid);
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

list<Action> ComportamientoIngeniero::B_Anchura(const EstadoI &inicio, const EstadoI &fin,
                                                const vector<vector<unsigned char>> &terreno,
                                                const vector<vector<unsigned char>> &altura) {

  NodoI current_node;
  current_node.estado = inicio;

  cout << "Inicio: " << current_node.estado.site.f << ", " << current_node.estado.site.c << endl;

  list<NodoI> frontier;
  set<NodoI> explored;
  list<Action> path;

  bool SolutionFound = (current_node.estado.site.f == fin.site.f &&
                        current_node.estado.site.c == fin.site.c);

  frontier.push_back(current_node);

  while (!frontier.empty() and !SolutionFound) {
    current_node = frontier.front();
    frontier.pop_front();
    explored.insert(current_node);

    if (terreno[current_node.estado.site.f][current_node.estado.site.c] == 'D') {
      current_node.estado.zapatillas = true;
    }

    if (!SolutionFound) {
 	  NodoI child_Jump = current_node;
  	  child_Jump.estado = applyI(JUMP, current_node.estado, terreno, altura);

      if (child_Jump.estado.site.f == fin.site.f and
        child_Jump.estado.site.c == fin.site.c) {
        child_Jump.secuencia.push_back(JUMP);
        current_node = child_Jump;
        SolutionFound = true;
      } else if (explored.find(child_Jump) == explored.end()) {
        child_Jump.secuencia.push_back(JUMP);
        frontier.push_back(child_Jump);
      }
    }

    if (!SolutionFound) {
 	  NodoI child_Walk = current_node;
      child_Walk.estado = applyI(WALK, current_node.estado, terreno, altura);

      if (child_Walk.estado.site.f == fin.site.f and
        child_Walk.estado.site.c == fin.site.c) {
        child_Walk.secuencia.push_back(WALK);
        current_node = child_Walk;
        SolutionFound = true;
      } else if (explored.find(child_Walk) == explored.end()) {
        child_Walk.secuencia.push_back(WALK);
        frontier.push_back(child_Walk);
      }
    }

    if (!SolutionFound) {
      NodoI child_TurnSR = current_node;
      child_TurnSR.estado = applyI(TURN_SR, current_node.estado, terreno, altura);

      if (explored.find(child_TurnSR) == explored.end()) {
        child_TurnSR.secuencia.push_back(TURN_SR);
        frontier.push_back(child_TurnSR);
      }

      NodoI child_TurnSL = current_node;
      child_TurnSL.estado = applyI(TURN_SL, current_node.estado, terreno, altura);

      if (explored.find(child_TurnSL) == explored.end()) {
        child_TurnSL.secuencia.push_back(TURN_SL);
        frontier.push_back(child_TurnSL);
      }
    }

    if (!SolutionFound and !frontier.empty()) {
      current_node = frontier.front();
      while (explored.find(current_node) != explored.end() and !frontier.empty()) {
        frontier.pop_front();
        current_node = frontier.front();
      }
    }
  }

  if (SolutionFound)
    path = current_node.secuencia;

  return path;
}

/**
 * @brief Comportamiento del ingeniero para el Nivel 2 (búsqueda).
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_2(Sensores sensores)
{
  Action accion = IDLE;

  if (!hayPlan) {
    EstadoI inicio, fin;
    inicio.site.f = sensores.posF;
    inicio.site.c = sensores.posC;
    inicio.site.brujula = sensores.rumbo;
    inicio.zapatillas = tiene_zapatillas;
    fin.site.f = sensores.BelPosF;
    fin.site.c = sensores.BelPosC;
    plan = B_Anchura(inicio, fin, mapaResultado, mapaCotas);
    VisualizaPlan(inicio.site,plan);
    hayPlan = plan.size() != 0;
  }

  if (hayPlan and plan.size() > 0) {
    accion = plan.front();
    plan.pop_front();
  }

  if (plan.size() == 0) {
    hayPlan = false;
  }

  return accion;
}

// ─────────────────────────────────────────────────────────────────────────────
// COMIENZO NIVEL 3 INGENIERO
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Comportamiento del ingeniero para el Nivel 3.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_3(Sensores sensores)
{
  Action accion = IDLE;

  if (pasos_evasion > 0) {
    pasos_evasion--;

    if (sensores.agentes[2] != 't' &&
      sensores.superficie[2] != 'P' &&
      sensores.superficie[2] != 'M' &&
      sensores.superficie[2] != 'B') {
      ubicacion actual;
      actual.f = sensores.posF;
      actual.c = sensores.posC;
      actual.brujula = sensores.rumbo;

      if (EsAccesiblePorAltura(actual, tiene_zapatillas)) {
        return last_action;
      }
    }

    return TURN_SR;
  }

  if (comprobando_obstaculiza) {
    if (sensores.agentes[2] == 't' || sensores.agentes[6] == 't') {
      accion = TURN_SL;
      pasos_evasion = 2;
    } else if (sensores.agentes[1] == 't' || sensores.agentes[4] == 't' || sensores.agentes[5] == 't') {
      accion = TURN_SR;
      pasos_evasion = 2;
    } else if (sensores.agentes[3] == 't' || sensores.agentes[7] == 't' || sensores.agentes[8] == 't') {
      accion = TURN_SL;
      pasos_evasion = 2;
    }
  }

  comprobando_obstaculiza = (comprobando_obstaculiza+1)%2;
  last_action = accion;

  return accion;
}

// ─────────────────────────────────────────────────────────────────────────────
// COMIENZO NIVEL 4
// ─────────────────────────────────────────────────────────────────────────────

int ComportamientoIngeniero::CosteInstallEnergia(unsigned char t) {
    switch (t) {
        case 'A': return 60;
        case 'H': return 45;
        case 'S': return 25;
        case 'C': return 15;
        case 'U': return 15;
        default: return 30;
    }
}

int ComportamientoIngeniero::CosteInstallEco(unsigned char t) {
    switch (t) {
        case 'A': return 50;
        case 'H': return 45;
        case 'S': return 25;
        case 'C': return 15;
        case 'U': return 15;
        default: return 30;
    }
}

int ComportamientoIngeniero::CosteRaise(unsigned char t) {
    switch (t) {
        case 'H': return 55;
        case 'S': return 30;
        case 'C': return 10;
        case 'U': return 10;
        default: return 40;
    }
}

int ComportamientoIngeniero::CosteDig(unsigned char t) {
    switch (t) {
        case 'H': return 65;
        case 'S': return 40;
        case 'C': return 25;
        case 'U': return 25;
        default: return 50;
    }
}

list<Paso> ComportamientoIngeniero::PlanificarTuberias(int belF, int belC, int max_energia, int max_eco) {

    const int df[] = {-1, 1,  0, 0};
    const int dc[] = { 0, 0,  1,-1};

    int filas    = mapaResultado.size();
    int columnas = mapaResultado[0].size();

    list<NodoBFS4> frontier;

    map<EstadoTuberia, int> min_energia_llegada;
    map<EstadoTuberia, int> min_eco_llegada;

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
            NodoBFS4 nodo;
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

        NodoBFS4 current = frontier.front();
        frontier.pop_front();

        int f  = current.estado.f;
        int c  = current.estado.c;
        int op = current.estado.op;
        int cotaActual = (int)mapaCotas[f][c] + op;

        // --- META ALCANZADA (BLOQUE DE LOGS AMPLIADO) ---
        if (mapaResultado[f][c] == 'U') {
            cout << "\n=========================================================================\n";
            cout << "¡PLAN VALIDO ENCONTRADO!\n";
            cout << "Tramos totales: " << current.camino.size() << "\n";
            cout << "Energia total gastada: " << current.energia_gastada << " / " << max_energia << "\n";
            cout << "Impacto ecologico total: " << current.eco_gastado << " / " << max_eco << "\n";
            cout << "-------------------------------------------------------------------------\n";
            cout << "DESGLOSE DEL CAMINO PASO A PASO:\n";

            int i = 0;
            for (const Paso &p : current.camino) {
                unsigned char terreno = mapaResultado[p.fil][p.col];
                int alt_orig = (int)mapaCotas[p.fil][p.col];
                int alt_final = alt_orig + p.op;

                string nombre_op = " 0 (NADA) ";
                if (p.op == 1)  nombre_op = " 1 (RAISE)";
                if (p.op == -1) nombre_op = "-1 (DIG)  ";

                cout << " [" << (i < 10 ? " " : "") << i << "] " // Formateo bonito para alinear
                     << "pos:(" << p.fil << "," << p.col << ")\t"
                     << "| ter: " << terreno << "\t"
                     << "| alt: " << alt_orig << "->" << alt_final << "\t"
                     << "| op: " << nombre_op << "\t"
                     << "| Coste Paso -> E: " << p.energia_paso << ", Eco: " << p.eco_paso
                     << endl;
                i++;
            }
            cout << "=========================================================================\n\n";

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

                    EstadoTuberia nextState = {nf, nc, onext};

                    bool mejor_energia = min_energia_llegada.find(nextState) == min_energia_llegada.end() || nueva_energia < min_energia_llegada[nextState];
                    bool mejor_eco = min_eco_llegada.find(nextState) == min_eco_llegada.end() || nuevo_eco < min_eco_llegada[nextState];

                    if (mejor_energia || mejor_eco) {
                        if (mejor_energia) min_energia_llegada[nextState] = nueva_energia;
                        if (mejor_eco) min_eco_llegada[nextState] = nuevo_eco;

                        NodoBFS4 child = current;
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
 * @brief Comportamiento del ingeniero para el Nivel 4.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_4(Sensores sensores) {

  if (!hayPlanPasosTuberias) {
    planPasosTuberias = PlanificarTuberias(
    sensores.BelPosF,
    sensores.BelPosC,
    sensores.energia,
    sensores.max_ecologico
    );

    VisualizaRedTuberias(planPasosTuberias);

    hayPlanPasosTuberias = true;
  }

  return IDLE;
}

// ─────────────────────────────────────────────────────────────────────────────
// COMIENZO NIVEL 5
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Comportamiento del ingeniero para el Nivel 5.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_5(Sensores sensores) {
	Action accion = IDLE;

    // Si pisamos unas zapatillas, actualizamos nuestro estado
    if (sensores.superficie[0] == 'D') {
        tiene_zapatillas = true;
    }

    switch(estadoAgente) {

        case 0: // FASE 0: PLANIFICANDO LA RED
          cout << "Ingeniero PLANIFICANDO" << endl;
            if (!hayPlanPasosTuberias) {
                // Generamos el plan con los límites de energía y ecología
                planPasosTuberias = PlanificarTuberias(
    			sensores.BelPosF,
    			sensores.BelPosC,
    			sensores.energia,
    			sensores.max_ecologico
    			);
                VisualizaRedTuberias(planPasosTuberias);
                hayPlanPasosTuberias = true;
                tramo_actual = 0;
            }
            // Si el plan tiene al menos 2 casillas (origen y destino), pasamos a movernos
            if (planPasosTuberias.size() > 1) {
                estadoAgente = 1;
                hayPlan = false; // Reseteamos el plan de movimiento por si acaso
            }
            break;

        case 1: { // FASE 1: DESPLAZANDOSE AL TRAMO ACTUAL
          cout << "Ingeniero DESPLAZANDOSE" << endl;
            auto it = planPasosTuberias.begin();
            std::advance(it, tramo_actual);
            int destF = it->fil;
            int destC = it->col;

            // ¿Ya estamos en la casilla origen del tramo?
            if (sensores.posF == destF && sensores.posC == destC) {
                estadoAgente = 2; // Siguiente fase: Ajustar cota
            } else {
                // Si no estamos, usamos el BFS del Nivel 2 para llegar
                if (!hayPlan) {
                    EstadoI inicio, fin;
                    inicio.site.f = sensores.posF;
                    inicio.site.c = sensores.posC;
                    inicio.site.brujula = sensores.rumbo;
                    inicio.zapatillas = tiene_zapatillas;
                    fin.site.f = destF;
                    fin.site.c = destC;

                    plan = B_Anchura(inicio, fin, mapaResultado, mapaCotas);
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

        case 2: { // FASE 2: AJUSTANDO LA COTA (RAISE / DIG)
          cout << "Ingeniero AJUSTANDO COTA" << endl;
            auto it = planPasosTuberias.begin();
            std::advance(it, tramo_actual);
            int op = it->op;

            // Si hay que modificar el terreno y aún no lo hemos hecho
            if (!cota_ajustada && op != 0) {
                cota_ajustada = true;
                if (op == 1) accion = RAISE;
                else if (op == -1) accion = DIG;
            } else {
                // Si no hay que tocar nada o ya lo tocamos, pasamos a esperar
                estadoAgente = 3;
            }
            break;
        }

        case 3: { // FASE 3: ORIENTARSE Y ESPERAR AL TÉCNICO
          cout << "Ingeniero ORIENTANDOSE" << endl;
            auto it = planPasosTuberias.begin();
            std::advance(it, tramo_actual);
            int curF = it->fil;
            int curC = it->col;

            std::advance(it, 1);
            int nextF = it->fil;
            int nextC = it->col;

            // 1. Calcular hacia dónde tenemos que mirar (Norte, Sur, Este u Oeste)
            Orientacion target = norte;
            if (nextF < curF) target = norte;
            else if (nextF > curF) target = sur;
            else if (nextC > curC) target = este;
            else if (nextC < curC) target = oeste;

            // 2. Si no estamos mirando hacia allí, giramos
            if (sensores.rumbo != target) {
                int diff = (target - sensores.rumbo + 8) % 8;
                if (diff <= 4) accion = TURN_SR;
                else accion = TURN_SL;
            } else {
                // 3. Ya estamos mirando a la casilla destino. ¿Está el técnico enfrente?
                if (sensores.enfrente) {
                    estadoAgente = 4; // ¡Listo para instalar!
                } else {
                    // Si no está, lo llamamos con COME
                    if (!he_llamado_tecnico) {
                        accion = COME;
                        he_llamado_tecnico = true;
                    } else {
                        accion = IDLE; // Esperamos pacientemente sin gastar energía[cite: 3]
                    }
                }
            }
            break;
        }

        case 4: { // FASE 4: INSTALACIÓN SIMULTÁNEA
          cout << "Ingeniero INSTALANDO?" << sensores.enfrente << endl;
          if (sensores.enfrente) {
            accion = INSTALL;
            tramo_actual++; // Avanzamos el progreso de la obra

            if (tramo_actual >= planPasosTuberias.size() - 1) {
              estadoAgente = 5; // ¡Obra terminada!
            } else {
              // Reseteamos variables para el siguiente tramo
              estadoAgente = 1;
              he_llamado_tecnico = false;
              cota_ajustada = false;
              hayPlan = false;
            }
          }
        break;
        }

        case 5: // FASE 5: FIN DEL JUEGO
          cout << "Ingeniero FIN" << endl;
            accion = IDLE;
            break;
    }

    return accion;
}

/**
 * @brief Comportamiento del ingeniero para el Nivel 6.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
Action ComportamientoIngeniero::ComportamientoIngenieroNivel_6(Sensores sensores)
{
  return IDLE;
}

// =========================================================================
// FUNCIONES PROPORCIONADAS
// =========================================================================

/**
 * @brief Actualiza el mapaResultado y mapaCotas con la información de los sensores.
 * @param sensores Datos actuales de los sensores.
 */
void ComportamientoIngeniero::ActualizarMapa(Sensores sensores)
{
  mapaResultado[sensores.posF][sensores.posC] = sensores.superficie[0];
  mapaCotas[sensores.posF][sensores.posC] = sensores.cota[0];

  int pos = 1;
  switch (sensores.rumbo)
  {
  case norte:
    for (int j = 1; j < 4; j++)
      for (int i = -j; i <= j; i++)
      {
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
      for (int i = -j; i <= j; i++)
      {
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
      for (int i = -j; i <= j; i++)
      {
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
      for (int i = -j; i <= j; i++)
      {
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
 * @brief Determina si una casilla es transitable para el ingeniero.
 * @param f Fila de la casilla.
 * @param c Columna de la casilla.
 * @param tieneZapatillas Indica si el agente posee las zapatillas.
 * @return true si la casilla es transitable (no es muro ni precipicio).
 */
bool ComportamientoIngeniero::EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas)
{
  if (f < 0 || f >= mapaResultado.size() || c < 0 || c >= mapaResultado[0].size())
    return false;
  return es_camino(mapaResultado[f][c]); // Solo 'C', 'D', 'U' son transitables en Nivel 0
}

/**
 * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
 * Para el ingeniero: desnivel máximo 1 sin zapatillas, 2 con zapatillas.
 * @param actual Estado actual del agente (fila, columna, orientacion, zap).
 * @return true si el desnivel con la casilla de delante es admisible.
 */
bool ComportamientoIngeniero::EsAccesiblePorAltura(const ubicacion &actual, bool zap)
{
  ubicacion del = Delante(actual);
  if (del.f < 0 || del.f >= mapaCotas.size() || del.c < 0 || del.c >= mapaCotas[0].size())
    return false;
  int desnivel = abs(mapaCotas[del.f][del.c] - mapaCotas[actual.f][actual.c]);
  if (zap && desnivel > 2)
    return false;
  if (!zap && desnivel > 1)
    return false;
  return true;
}

/**
 * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
 * Calcula la casilla frontal según la orientación actual (8 direcciones).
 * @param actual Estado actual del agente (fila, columna, orientacion).
 * @return Estado con la fila y columna de la casilla de enfrente.
 */
ubicacion ComportamientoIngeniero::Delante(const ubicacion &actual) const
{
  ubicacion delante = actual;
  switch (actual.brujula)
  {
  case 0:
    delante.f--;
    break; // norte
  case 1:
    delante.f--;
    delante.c++;
    break; // noreste
  case 2:
    delante.c++;
    break; // este
  case 3:
    delante.f++;
    delante.c++;
    break; // sureste
  case 4:
    delante.f++;
    break; // sur
  case 5:
    delante.f++;
    delante.c--;
    break; // suroeste
  case 6:
    delante.c--;
    break; // oeste
  case 7:
    delante.f--;
    delante.c--;
    break; // noroeste
  }
  return delante;
}

ubicacion ComportamientoIngeniero::Izquierda(const ubicacion &actual) const
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

ubicacion ComportamientoIngeniero::Derecha(const ubicacion &actual) const
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
void ComportamientoIngeniero::PintaPlan(const list<Action> &plan)
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
 * @brief Imprime las coordenadas y operaciones de un plan de tubería.
 *
 * @param plan  Lista de pasos (fila, columna, operación),
 *              donde operacion = -1 (DIG), operación = 1 (RAISE).
 */
void ComportamientoIngeniero::PintaPlan(const list<Paso> &plan)
{
  auto it = plan.begin();
  while (it != plan.end())
  {
    cout << it->fil << ", " << it->col << " (" << it->op << ")\n";
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
void ComportamientoIngeniero::VisualizaPlan(const ubicacion &st,
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

/**
 * @brief Convierte un plan de tubería en la lista de casillas usada
 *        por el sistema de visualización.
 *
 * @param st    Estado de partida (no utilizado directamente).
 * @param plan  Lista de pasos del plan de tubería.
 */
void ComportamientoIngeniero::VisualizaRedTuberias(const list<Paso> &plan)
{
  listaCanalizacionTuberias.clear();
  auto it = plan.begin();
  while (it != plan.end())
  {
    listaCanalizacionTuberias.push_back({it->fil, it->col, it->op});
    it++;
  }
}
