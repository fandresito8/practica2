#ifndef COMPORTAMIENTOTECNICO_H
#define COMPORTAMIENTOTECNICO_H

#include <chrono>
#include <time.h>
#include <thread>
#include <list>

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
 * 
 * 
 * DIFERENCIAS CON EL INGENIERO:
 * - El técnico NO tiene capacidades especiales
 * - El técnico puede acceder al bosque cuando tiene las zapatillas
 * - Colabora con el ingeniero mediante comunicación (sensores.venpaca, sensores.enfrente)
 * 
 * ACCIONES ESPECIALES DEL TÉCNICO:
 * - INSTALL: Instalar tuberías o equipos
 * 
 * COMUNICACIÓN CON EL INGENIERO:
 * - sensores.venpaca: Indica que el ingeniero necesita al técnico
 * - sensores.enfrente: Indica que el ingeniero está enfrente (orientaciones opuestas)
 * - sensores.GotoF, sensores.GotoC: Coordenadas destino enviadas por el ingeniero
 */

struct estadoT {
  int fila;
  int columna;
  int orientacion;
  bool zap;
  bool operator<(const estadoT &n) const {
    /* 
     * TAREA DEL ESTUDIANTE: Redefinir el operador menor para usar estados en contenedores ordenados.
     * Ejemplo: std::set<estadoT> o std::map<estadoT, valor>
     * 
     * PISTA: Un estado es "menor" que otro si alguna de sus componentes es menor,
     * comparando en orden: fila, columna, orientacion, zap.
     */
    if (fila < n.fila) 
      return true;
    else
      return false;
  }
  bool operator==(const estadoT &n) const {
    return (fila == n.fila and columna == n.columna and
            orientacion == n.orientacion and zap == n.zap);
  }
};

struct ComparaEstadosT {
  bool operator()(const estadoT &a, const estadoT &n) const {
    /* 
     * TAREA DEL ESTUDIANTE: Functor alternativo para comparar estados.
     * Útil para colas de prioridad (priority_queue).
     * 
     * PISTA: En priority_queue, el elemento "más grande" según el comparador
     * es el que tiene mayor prioridad (se saca primero).
     */
    if (a.fila > n.fila)
      return true;
    else
      return false;
  }
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
  ComportamientoTecnico(std::vector<std::vector<unsigned char>> mapaR, 
                       std::vector<std::vector<unsigned char>> mapaC): 
                       Comportamiento(mapaR, mapaC) {
    // Inicializar Variables de Estado
    last_action = IDLE;
    tiene_zapatillas = false;
    instante = 0;
  }

  ComportamientoTecnico(const ComportamientoTecnico &comport): Comportamiento(comport) {}
  ~ComportamientoTecnico() {}

  /**
   * @brief Bucle principal de decisión del técnico.
   * Estudia los sensores y decide la siguiente acción.
   * 
   * EJEMPLO DE USO:
   * Action accion = think(sensores);
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
   * @brief Calcula el coste de realizar una acción desde un estado determinado.
   * @param origen Estado actual del agente.
   * @param accion Acción que se pretende realizar.
   * @return Coste de la acción (p.e. basado en el tipo de terreno).
   * 
   * NOTA: El técnico puede tener costes diferentes al ingeniero para las mismas acciones.
   */
  int CosteDeLaAccion(const estadoT &origen, Action accion);

  /**
   * @brief Algoritmo de búsqueda de caminos para el técnico.
   * @param origen Estado inicial.
   * @param destino Estado objetivo.
   * @param plan Lista de acciones resultante (se llena por referencia).
   * @return true si se encontró un camino, false en caso contrario.
   * 
   * CONSIDERACIONES ESPECIALES PARA EL TÉCNICO:
   * - Puede usar acciones especiales (DIG, RAISE) para modificar el terreno
   * - Debe considerar la colaboración con el ingeniero
   */
  bool pathFinding(const estadoT &origen, const estadoT &destino, std::list<Action> &plan);

  // Funciones específicas para cada nivel del técnico
  
  /**
   * @brief Nivel 0: Comportamiento reactivo básico.
   * OBJETIVO: Alcanzar una casilla 'U' (objetivo) priorizando caminos.
   * ESTRATEGIA: Similar al ingeniero nivel 0.
   */
  Action ComportamientoTecnicoNivel_0(Sensores sensores);
  
  /**
   * @brief Nivel 1: Comportamiento reactivo mejorado.
   * OBJETIVO: Mejorar el nivel 0 considerando más sensores.
   * ESTRATEGIA: Usar sensores adicionales para mejores decisiones.
   */
  Action ComportamientoTecnicoNivel_1(Sensores sensores);
  
  /**
   * @brief Nivel 2: Búsqueda con información completa.
   * OBJETIVO: Encontrar camino óptimo al objetivo usando pathFinding.
   * CONDICIÓN: Se conoce todo el mapa.
   */
  Action ComportamientoTecnicoNivel_2(Sensores sensores);
  
  /**
   * @brief Nivel 3: Búsqueda con información parcial.
   * OBJETIVO: Explorar mapa desconocido y encontrar objetivos.
   * ESTRATEGIA: Combinar exploración con búsqueda.
   */
  Action ComportamientoTecnicoNivel_3(Sensores sensores);
  
  /**
   * @brief Nivel 4: Colaboración básica con ingeniero.
   * OBJETIVO: Responder a llamadas del ingeniero (sensores.venpaca).
   * ESTRATEGIA: Ir a las coordenadas indicadas por el ingeniero.
   */
  Action ComportamientoTecnicoNivel_4(Sensores sensores);
  
  /**
   * @brief Nivel 5: Colaboración intermedia.
   * OBJETIVO: Realizar tareas conjuntas con el ingeniero.
   * ESTRATEGIA: Coordinar acciones cuando están enfrente (sensores.enfrente).
   */
  Action ComportamientoTecnicoNivel_5(Sensores sensores);
  
  /**
   * @brief Nivel 6: Colaboración avanzada.
   * OBJETIVO: Resolver problemas complejos que requieren ambos agentes.
   * ESTRATEGIA: Planificación conjunta y uso de acciones especiales.
   */
  Action ComportamientoTecnicoNivel_6(Sensores sensores);

protected:
  // =========================================================================
  // PRIMITIVAS SENSORIALES-MOTORAS (PROPORCIONADAS)
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
  bool EsAccesiblePorAltura(const estadoT &actual);

  /**
   * @brief Devuelve la posición (fila, columna) de la casilla que hay delante del agente.
   * @param actual Estado actual del agente (fila, columna, orientacion).
   * @return Estado con la fila y columna de la casilla de enfrente.
   */
  estadoT Delante(const estadoT &actual) const;

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
  
  /// Última acción realizada con éxito por el técnico.
  Action last_action;
  
  /// Indica si el técnico posee actualmente las zapatillas.
  bool tiene_zapatillas;
  
  /// Contador de instantes del técnico.
  int instante;
  
};

#endif
