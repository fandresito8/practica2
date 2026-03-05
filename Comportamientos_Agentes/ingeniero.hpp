#ifndef COMPORTAMIENTOINGENIERO_H
#define COMPORTAMIENTOINGENIERO_H

#include <chrono>
#include <list>
#include <map>
#include <set>
#include <thread>
#include <time.h>

#include "comportamientos/comportamiento.hpp"

// =========================================================================
// DOCUMENTACIÓN PARA ESTUDIANTES
// =========================================================================
/*
 * CLASE: ComportamientoIngeniero
 * 
 * DESCRIPCIÓN:
 * Esta clase implementa el comportamiento del agente Ingeniero en el mundo Belkan.
 * El ingeniero es responsable de explorar el mapa, encontrar objetivos y colaborar
 * con el técnico para resolver problemas.
 * 
 * NIVELES DE COMPORTAMIENTO:
 * - Nivel 0: Comportamiento reactivo básico (parcialmente implementado)
 * - Nivel 1: Comportamiento reactivo mejorado
 * - Nivel 2: Búsqueda de caminos con información completa
 * - Nivel 3: Búsqueda con información parcial
 * - Nivel 4: Planificación con múltiples objetivos
 * - Nivel 5: Colaboración básica con el técnico
 * - Nivel 6: Colaboración avanzada
 * 
 * SENSORES IMPORTANTES:
 * - sensores.superficie[0]: Tipo de terreno en la casilla actual
 * - sensores.superficie[2]: Tipo de terreno en la casilla de delante
 * - sensores.posF, sensores.posC: Posición actual (fila, columna)
 * - sensores.rumbo: Orientación actual (0=norte, 1=noreste, ..., 7=noroeste)
 * - sensores.cota[0]: Altura de la casilla actual
 * 
 * TIPOS DE TERRENO:
 * 'C' = Camino (transitable)
 * 'S' = Sendero (transitable)
 * 'D' = Zapatillas (objeto especial)
 * 'U' = Objetivo/Meta
 * 'B' = Bosque (transitable con zapatillas)
 * 'A' = Agua (no transitable)
 * 'P' = Precipicio (no transitable)
 * 'M' = Montaña (no transitable)
 * 'H' = Hierba (transitable con coste alto)
 */

struct estadoI {
  int fila;
  int columna;
  int orientacion;
  bool zap;
  bool operator<(const estadoI &n) const {
    /* 
     * TAREA DEL ESTUDIANTE: Redefinir el operador menor para usar estados en contenedores ordenados.
     * Ejemplo: std::set<estadoI> o std::map<estadoI, valor>
     * 
     * PISTA: Un estado es "menor" que otro si alguna de sus componentes es menor,
     * comparando en orden: fila, columna, orientacion, zap.
     */
    if (fila < n.fila) 
      return true;
    else
      return false;
  }
  bool operator==(const estadoI &n) const {
    return (fila == n.fila and columna == n.columna and
            orientacion == n.orientacion and zap == n.zap);
  }
};

struct ComparaEstadosI {
  bool operator()(const estadoI &a, const estadoI &n) const {
    /* 
     * TAREA DEL ESTUDIANTE: Functor alternativo para comparar estados.
     * Útil para colas de prioridad (priority_queue).
     * 
     * PISTA: En priority_queue, el elemento "más grande" según el comparador
     * es el que tiene mayor prioridad (se saca primero).
     */
    if ((a.fila > n.fila))
      return true;
    else
      return false;
  }
};



class ComportamientoIngeniero : public Comportamiento {
public:
  // =========================================================================
  // CONSTRUCTORES
  // =========================================================================
  
  /**
   * @brief Constructor para niveles 0, 1 y 6 (sin mapa completo)
   * @param size Tamaño del mapa (si es 0, se inicializa más tarde)
   */
  ComportamientoIngeniero(unsigned int size = 0) : Comportamiento(size) {
    // Inicializar Variables de Estado
    last_action = IDLE;
    tiene_zapatillas = false;
    instante = 0;
  }

  /**
   * @brief Constructor para niveles 2, 3, 4 y 5 (con mapa completo conocido)
   * @param mapaR Mapa de terreno conocido
   * @param mapaC Mapa de cotas conocido
   */
  ComportamientoIngeniero(std::vector<std::vector<unsigned char>> mapaR, 
                         std::vector<std::vector<unsigned char>> mapaC): 
                         Comportamiento(mapaR, mapaC) {
    // Inicializar Variables de Estado
    last_action = IDLE;
    tiene_zapatillas = false;
    instante = 0;
  }

  ComportamientoIngeniero(const ComportamientoIngeniero &comport)
      : Comportamiento(comport) {}
  ~ComportamientoIngeniero() {}

  /**
   * @brief Bucle principal de decisión del agente.
   * Estudia los sensores y decide la siguiente acción.
   * 
   * EJEMPLO DE USO:
   * Action accion = think(sensores);
   * return accion; // El motor ejecutará esta acción
   */
  Action think(Sensores sensores);

  ComportamientoIngeniero *clone() {
    return new ComportamientoIngeniero(*this);
  }

  // =========================================================================
  // ÁREA DE IMPLEMENTACIÓN DEL ESTUDIANTE
  // =========================================================================

  /**
   * @brief Calcula el coste de realizar una acción desde un estado determinado.
   * @param origen Estado actual del agente.
   * @param accion Acción que se pretende realizar.
   * @return Coste de la acción (p.e. basado en el tipo de terreno).
   * 
   * EJEMPLO DE IMPLEMENTACIÓN:
   * - Terreno 'C' (camino): coste 1
   * - Terreno 'S' (sendero): coste 2
   * - Terreno 'B' (bosque): coste 5 sin zapatillas, 2 con zapatillas
   * - Terreno 'H' (arena): coste 3
   * - Acción JUMP: coste adicional 2
   */
  int CosteDeLaAccion(const estadoI &origen, Action accion);

  /**
   * @brief Algoritmo de búsqueda de caminos.
   * @param origen Estado inicial.
   * @param destino Estado objetivo.
   * @param plan Lista de acciones resultante (se llena por referencia).
   * @return true si se encontró un camino, false en caso contrario.
   * 
   * ALGORITMOS SUGERIDOS:
   * - BFS (Breadth-First Search): Para encontrar el camino más corto en pasos
   * - UCS (Uniform Cost Search): Para minimizar coste acumulado
   * - A* (A-star): Para búsqueda heurística eficiente
   * 
   * ESTRUCTURAS DE DATOS ÚTILES:
   * - std::queue<estadoI> para BFS
   * - std::priority_queue<estadoI> para UCS/A*
   * - std::map<estadoI, estadoI> para reconstruir el camino
   * - std::map<estadoI, int> para costes acumulados
   */
  bool pathFinding(const estadoI &origen, const estadoI &destino, std::list<Action> &plan);

  // Funciones específicas para cada nivel (para ser implementadas por el alumno)
  
  /**
   * @brief Implementación del comportamiento reactivo para el Nivel 0.
   * OBJETIVO: Alcanzar una casilla 'U' (objetivo) priorizando caminos 
   * y evitando bucles mediante el uso de la matriz de tiempo.
   * ESTRATEGIA: Seguir caminos ('C'), girar cuando no haya camino adelante.
   * @param sensores Datos actuales de los sensores del agente.
   * @return Acción a realizar.
   */
  Action ComportamientoIngenieroNivel_0(Sensores sensores);
  
  /**
   * @brief Nivel 1: Comportamiento reactivo mejorado.
   * OBJETIVO: Mejorar el nivel 0 considerando más sensores y evitando callejones.
   * ESTRATEGIA: Usar sensores[1] (izquierda) y sensores[3] (derecha) para decisiones.
   */
  Action ComportamientoIngenieroNivel_1(Sensores sensores);
  
  /**
   * @brief Nivel 2: Búsqueda con información completa.
   * OBJETIVO: Encontrar camino óptimo al objetivo usando pathFinding.
   * CONDICIÓN: Se conoce todo el mapa (mapaResultado está completo).
   */
  Action ComportamientoIngenieroNivel_2(Sensores sensores);
  
  /**
   * @brief Nivel 3: Búsqueda con información parcial.
   * OBJETIVO: Explorar mapa desconocido y encontrar objetivos.
   * ESTRATEGIA: Combinar exploración con búsqueda hacia áreas desconocidas.
   */
  Action ComportamientoIngenieroNivel_3(Sensores sensores);
  
  /**
   * @brief Nivel 4: Planificación con múltiples objetivos.
   * OBJETIVO: Visitar varios objetivos en orden óptimo.
   * ESTRATEGIA: Algoritmo de planificación de rutas (TSP simple).
   */
  Action ComportamientoIngenieroNivel_4(Sensores sensores);
  
  /**
   * @brief Nivel 5: Colaboración básica con técnico.
   * OBJETIVO: Coordinar acciones con el técnico para tareas conjuntas.
   * COMUNICACIÓN: Usar sensores.venpaca y sensores.GotoF/GotoC.
   */
  Action ComportamientoIngenieroNivel_5(Sensores sensores);
  
  /**
   * @brief Nivel 6: Colaboración avanzada.
   * OBJETIVO: Resolver problemas complejos que requieren ambos agentes.
   * ESTRATEGIA: Planificación conjunta y comunicación bidireccional.
   */
  Action ComportamientoIngenieroNivel_6(Sensores sensores);

protected:
  // =========================================================================
  // PRIMITIVAS SENSORIALES-MOTORAS (PROPORCIONADAS)
  // =========================================================================

  /**
   * @brief Actualiza la información del mapa interno basándose en los sensores.
   * IMPORTANTE: Esta función ya está implementada. Actualiza mapaResultado y mapaCotas
   * con la información de los 16 sensores (casilla actual + 15 casillas alrededor).
   */
  void ActualizarMapa(Sensores sensores);

  /**
   * @brief Comprueba si una casilla es transitable.
   * @param f Fila de la casilla.
   * @param c Columna de la casilla.
   * @param tieneZapatillas Indica si el agente posee zapatillas.
   * @return true si la casilla es transitable (no es muro ni precipicio).
   */
  bool EsCasillaTransitableLevel0(int f, int c, bool tieneZapatillas);

  /**
   * @brief Comprueba si la casilla de delante es accesible por diferencia de altura.
   * REGLAS: Desnivel máximo 1 sin zapatillas, 2 con zapatillas.
   * @param actual Estado actual del agente (fila, columna, orientacion).
   * @return true si el desnivel con la casilla de delante es admisible.
   */
  bool EsAccesiblePorAltura(const estadoI &actual);

  /**
   * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
   * @param actual Estado actual del agente (fila, columna, orientacion).
   * @return Estado con la fila y columna de la casilla de enfrente.
   */
  estadoI Delante(const estadoI &actual) const;

  /**
   * @brief Comprueba si una celda es de tipo transitable por defecto (camino, sendero, etc).
   * @param c Carácter que representa el tipo de superficie.
   * @return true si es una casilla de tipo camino ('C', 'S', 'D' o 'U').
   */
  bool es_camino(unsigned char c) const;


private:
  // =========================================================================
  // VARIABLES DE ESTADO (PUEDEN SER EXTENDIDAS POR EL ALUMNO)
  // =========================================================================
  
  /// Última acción realizada con éxito por el agente.
  Action last_action;
  
  /// Indica si el ingeniero posee actualmente las zapatillas.
  bool tiene_zapatillas;
  
  /// Contador global de instantes de la simulación.
  int instante;

};

#endif
