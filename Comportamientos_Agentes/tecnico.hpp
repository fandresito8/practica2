#ifndef COMPORTAMIENTOTECNICO_H
#define COMPORTAMIENTOTECNICO_H

#include <chrono>
#include <time.h>
#include <thread>
#include <list>
#include <set>
#include <climits>

#include "comportamientos/comportamiento.hpp"

// =========================================================================
// DOCUMENTACIÓN PARA ESTUDIANTES
// =========================================================================
/*
 * CLASE: ComportamientoTecnico
 * 
 * DESCRIPCIÓN:
 * Esta clase implementa el comportamiento del agente Técnico en el mundo Belkan.
 * El técnico colabora con el ingeniero para resolver el problema de instalación de tuberías
 */

struct EstadoT {
  ubicacion site;
  bool zapatillas;

  bool operator==(const EstadoT &st) const {
    return site == st.site and zapatillas == st.zapatillas;
  }

  bool operator<(const EstadoT &st) const {
    if (site.f < st.site.f) return true;
    if (site.f == st.site.f && site.c < st.site.c) return true;
    if (site.f == st.site.f && site.c == st.site.c && site.brujula < st.site.brujula) return true;
    if (site.f == st.site.f && site.c == st.site.c &&
        site.brujula == st.site.brujula && zapatillas < st.zapatillas) return true;
    return false;
  }
};

struct NodoT {
  EstadoT estado;
  list<Action> secuencia;
  int g; // coste acumulado
  int h; // heurística

  bool operator==(const NodoT &node) const {
    return estado == node.estado;
  }
};

struct Comparador {
    bool operator()(const NodoT &a, const NodoT &b) const {
        return (a.g + a.h) > (b.g + b.h); // menor f = mayor prioridad
    }
};

struct EstadoTuberiaT {
  int f;
  int c;
  int op; // La operación aplicada: -1 (DIG), 0 (Nada), 1 (RAISE)

  bool operator<(const EstadoTuberiaT &o) const {
    if (f != o.f) return f < o.f;
    if (c != o.c) return c < o.c;
    return op < o.op;
  }
};

struct NodoBFS4T {
  EstadoTuberiaT estado;
  list<Paso> camino;
  int energia_gastada;
  int eco_gastado;
};

enum EstadoTecnicoN5 {
  ESPERANDO_LLAMADA,
  NAVEGANDO,
  ESPERANDO_INSTALACION,
  INSTALANDO_TEC
};

class ComportamientoTecnico : public Comportamiento {
public:
  // =========================================================================
  // CONSTRUCTORES
  // =========================================================================
  
  /**
   * @brief Constructor para niveles 0, 1 y 6 (sin mapa completo)
   * @param size Tamaño del mapa (si es 0, se inicializa más tarde)
   */
  ComportamientoTecnico(unsigned int size = 0) : Comportamiento(size) {
    last_action = IDLE;
    tiene_zapatillas = false;
    pasos_desde_ultima_visita = 0;
    giros_consecutivos = 0;
    pasos_evasion = 0;
    gira_izq = true;
  }

  /**
   * @brief Constructor para niveles 2, 3, 4 y 5 (con mapa completo conocido)
   * @param mapaR Mapa de terreno conocido
   * @param mapaC Mapa de cotas conocido
   */
  ComportamientoTecnico(std::vector<std::vector<unsigned char>> mapaR, 
                       std::vector<std::vector<unsigned char>> mapaC): 
                       Comportamiento(mapaR, mapaC) {
    hayPlan = false;
    tiene_zapatillas = false;
    comprobando_obstaculiza = true;

    estadoTecnico = 0;
    tramo_actual = 0;
    plan_calculado = false;
  }

  ComportamientoTecnico(const ComportamientoTecnico &comport): Comportamiento(comport) {}
  ~ComportamientoTecnico() {}

  /**
   * @brief Bucle principal de decisión del técnico.
   * Estudia los sensores y decide la siguiente acción.
   * 
   * EJEMPLO DE USO:
   * Action accion = think(sensores);ComportamientoTecnicoNivelComportamientoTecnicoNivel
   * return accion; // El motor ejecutará esta acción
   */
  Action think(Sensores sensores);

  ComportamientoTecnico *clone() {
    return new ComportamientoTecnico(*this);
  }

  // =========================================================================
  // ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
  // =========================================================================
  
/**
 * @brief Comportamiento del técnico para el Nivel 0.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_0(Sensores sensores);
  
/**
 * @brief Comportamiento del técnico para el Nivel 1.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_1(Sensores sensores);

/**
 * @brief Comportamiento del técnico para el Nivel 2.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_2(Sensores sensores);

  void AnularMatrizA(vector<vector<unsigned char>> &m);

  void VisualizaPlan(const EstadoT &st, const list<Action> &plan);

/**
 * @brief Comportamiento del técnico para el Nivel 3.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_3(Sensores sensores);
  
/**
 * @brief Comportamiento del técnico para el Nivel 4.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_4(Sensores sensores);
  
/**
 * @brief Comportamiento del técnico para el Nivel 5.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_5(Sensores sensores);
  
/**
 * @brief Comportamiento del técnico para el Nivel 6.
 * @param sensores Datos actuales de los sensores.
 * @return Acción a realizar.
 */
  Action ComportamientoTecnicoNivel_6(Sensores sensores);

protected:
  // =========================================================================
  // FUNCIONES PROPORCIONADAS
  // =========================================================================

  /**
   * @brief Actualiza el mapaResultado y mapaCotas con la información de los sensores.
   * IMPORTANTE: Esta función ya está implementada. Actualiza mapaResultado y mapaCotas
   * con la información de los 16 sensores.
   */
  void ActualizarMapa(Sensores sensores);

  /**
   * @brief Determina si una casilla es transitable para el técnico.
   * NOTA: El técnico puede tener reglas de transitabilidad diferentes al ingeniero.
   * @param f Fila de la casilla.
   * @param c Columna de la casilla.
   * @param tieneZapatillas Indica si el agente posee las zapatillas.
   * @return true si la casilla es transitable.
   */
  bool EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas);

  /**
   * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
   * REGLA PARA TÉCNICO: Desnivel máximo siempre 1 (independiente de zapatillas).
   * @param actual Estado actual del agente (fila, columna, orientacion).
   * @return true si el desnivel con la casilla de delante es admisible.
   */
  bool EsAccesiblePorAltura(const ubicacion &actual);

  /**
   * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
   * @param actual Estado actual del agente (fila, columna, orientacion).
   * @return Estado con la fila y columna de la casilla de enfrente.
   */
  ubicacion Delante(const ubicacion &actual) const;

    /**
   * @brief Devuelve la posición (fila, columna) de la casilla diagonal izquierda respecto al agente.
   * @param actual Estado actual del agente (fila, columna, orientacion).
   * @return Estado con la fila y columna de la casilla diagonal izquierda.
   */
  ubicacion Izquierda(const ubicacion &actual) const;

  /**
   * @brief Devuelve la posición (fila, columna) de la casilla diagonal derecha respecto al agente.
   * @param actual Estado actual del agente (fila, columna, orientacion).
   * @return Estado con la fila y columna de la casilla diagonal derecha.
   */
  ubicacion Derecha(const ubicacion &actual) const;

  EstadoT NextCasillaTecnico(const EstadoT &st);

  bool CasillaAccesibleTecnico(const EstadoT &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);

  EstadoT applyT(Action accion, const EstadoT & st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);

  void PintaPlan(const list<Action> &plan, bool zap);

  list<Action> A_Star(const EstadoT &inicio, const EstadoT &final,
                      const vector<vector<unsigned char>> &terreno,
                      const vector<vector<unsigned char>> &altura);

  int heuristica(const EstadoT &st, const EstadoT &fin);

  int costeTerreno(unsigned char t, bool zapatillas, int altura_origen = -1, int altura_destino = -1);

  list<Action> B_Anchura(const EstadoT &inicio, const EstadoT &final,
                         const vector<vector<unsigned char>> &terreno,
                         const vector<vector<unsigned char>> &altura);

  int CosteInstallEnergia(unsigned char t);

  int CosteInstallEco(unsigned char t);

  int CosteRaise(unsigned char t);

  int CosteDig(unsigned char t);

  list<Paso> PlanificarTuberias(int belF, int belC, int max_energia, int max_eco);

  /**
   * @brief Comprueba si una celda es de tipo transitable por defecto.
   * @param c Carácter que representa el tipo de superficie.
   * @return true si es camino ('C'), zapatillas ('D') o meta ('U').
   */
  bool es_camino(unsigned char c) const;

    /**
 * @brief Imprime por consola la secuencia de acciones de un plan para un agente.
 * @param plan  Lista de acciones del plan.
 */
  void PintaPlan(const list<Action> &plan);


/**
 * @brief Imprime las coordenadas y operaciones de un plan de tubería.
 * @param plan  Lista de pasos (fila, columna, operación).
 */
  void PintaPlan(const list<Paso> &plan);


  /**
 * @brief Convierte un plan de acciones en una lista de casillas para
 *        su visualización en el mapa gráfico.
 * @param st    Estado de partida.
 * @param plan  Lista de acciones del plan.
 */
  void VisualizaPlan(const ubicacion &st, const list<Action> &plan);

private:
  // =========================================================================
  // VARIABLES DE ESTADO (PUEDEN SER EXTENDIDAS POR EL ALUMNO)
  // =========================================================================
  Action last_action;
  bool tiene_zapatillas;
  vector<vector<int>> mapa_visitas;
  int pasos_desde_ultima_visita;
  int giros_consecutivos;
  int pasos_evasion;
  bool gira_izq;

  // Deliberativos
  list<Action> plan;
  bool hayPlan;
  bool comprobando_obstaculiza;

  // Nivel 5
  int estadoTecnico; // 0:PLANIFICAR, 1:ESPERAR_LLAMADA, 2:DESPLAZAR, 3:ENCARAR, 4:INSTALAR, 5:FIN
  int tramo_actual;
  list<Paso> planTuberiasTecnico;
  bool plan_calculado;
};

#endif
