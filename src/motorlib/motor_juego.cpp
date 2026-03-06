#include "motorlib.hpp"

#include <unistd.h>

extern MonitorJuego monitor;

bool actuacionIngeniero(unsigned char celdaJ_inicial, unsigned char celdaJ_fin,
                        int difAltJ, Action accion, unsigned int x,
                        unsigned int y) {
  int posibleElAvance;
  unsigned int mix, miy;
  unsigned char celdaRand;
  bool salida = false;
  int gasto;
  int error = 0; // 0 = NoError | 1 = NoEnergiaSuficiente | 2 = Colision

  // Primero evaluo si hay energía suficiente para realizar las acciones del
  // ingeniero.

  gasto = monitor.get_entidad(0)->fixBateria_sig_accion_jugador(
      celdaJ_inicial, difAltJ, accion);

  // std::cout << "Accion Ingeniero: " << accion << "   Accion Colaborador: " <<
  // accionColaborador << endl; std::cout << "Gasto Ingeniero: " <<
  // gasto_Ingeniero << "   Gasto Colaborador: " << gasto_colaborador << " Gasto
  // Total: " << gasto << endl;

  if (gasto > monitor.get_entidad(0)->getBateria()) {
    monitor.addMensaje("Ingeniero", "Falta energia");
    error = 1;
    monitor.get_entidad(0)->setBateria(0);

    return false;
  }

  switch (accion) {
  case WALK:
    if (monitor.getMapa()->casillaOcupada(0) == -1 and
        ((abs(difAltJ) <= 1) or
         (monitor.get_entidad(0)->Has_Zapatillas() and
          (abs(difAltJ) <= 2)))) // Casilla destino desocupada
                                 // monitor.get_entidad(0)->seAostio();
    {
      switch (celdaJ_fin) {
      case 'B': // Arbol
      case 'M': // Muro
        monitor.get_entidad(0)->seAostio();
        monitor.addCollisionMarker(x, y);
        if (celdaJ_fin == 'M')
          monitor.addMensaje("Ingeniero", "Choco con muro");
        else
          monitor.addMensaje("Ingeniero", "Choco con arbol");
        break;
      case 'P': // Precipicio
        monitor.addMensaje("Ingeniero", "Cayo al precipicio");
        monitor.get_entidad(0)->resetEntidad();
        monitor.get_entidad(0)->setHitbox(true);
        monitor.finalizarJuego();
        monitor.setMostrarResultados(true);
        break;
      case 'X': // Casilla Rosa (Puesto Base)
        monitor.get_entidad(0)->setPosicion(x, y);
        salida = true;
        break;
      case 'D': // Casilla Morada (Zapatillas)
        // tomo la zapatilla
        monitor.get_entidad(0)->Cogio_Zapatillas(true);
        monitor.get_entidad(0)->setPosicion(x, y);
        salida = true;
        break;
      default:
        monitor.get_entidad(0)->setPosicion(x, y);
        salida = true;
        break;
      }
      if (monitor.getLevel() > 1) {
        if (monitor.getLevel() == 2 and
            monitor.get_entidad(0)->isMemberObjetivo(x, y) != -1) {
          // acaba de completar todos los objetivos.
          monitor.addMensaje("Ingeniero", "Objetivo alcanzado");
          // El ingeniero llegó a la casilla objetivo.
          monitor.get_entidad(0)->setCompletoLosObjetivos();
          monitor.get_entidad(0)->incrMisiones();

          monitor.finalizarJuego();
          monitor.setMostrarResultados(true);
        } else if (monitor.getLevel() == 7 and
                   monitor.get_entidad(0)->isMemberObjetivo(x, y) != -1 ) {
          monitor.addMensaje("Ingeniero", "Objetivo alcanzado");
          monitor.get_entidad(0)->setCompletoLosObjetivos();
          monitor.put_active_objetivos(1);
          monitor.get_entidad(0)->anularAlcanzados();
          monitor.get_entidad(0)->incrMisiones();
          monitor.get_entidad(0)->incrPuntuacion(2);
          monitor.get_entidad(1)->AsignarCall_ON(
              false); // Se deja de llamar al tecnico

          for (unsigned int i = 0; i < monitor.numero_entidades(); i++) {
            monitor.get_entidad(i)->setObjetivos(
                monitor.get_active_objetivos());
          }
        } else if (monitor.getLevel() == 7 and
                   monitor.get_entidad(0)->isMemberObjetivo(x, y) != -1 and
                   monitor.CanHeSeesThisCell(
                       1, monitor.get_entidad(0)->getObjFil(0),
                       monitor.get_entidad(0)->getObjCol(0))) {
          monitor.put_active_objetivos(1);
          monitor.get_entidad(0)->anularAlcanzados();
          monitor.get_entidad(0)->incrMisiones();
          monitor.get_entidad(0)->incrPuntuacion(7);
          monitor.get_entidad(1)->AsignarCall_ON(
              false); // Se deja de llamar al tecnico

          for (unsigned int i = 0; i < monitor.numero_entidades(); i++) {
            monitor.get_entidad(i)->setObjetivos(
                monitor.get_active_objetivos());
          }
        }
      }
      // monitor.get_entidad(0)->fixBateria_sig_accion(celdaJ_inicial, accion);
    } else if (monitor.getMapa()->casillaOcupada(0) !=
               -1) // Choca contra otro agente
    {
      // Choca contra una entidad
      monitor.get_entidad(0)->seAostio();
      monitor.addCollisionMarker(x, y);
      if (monitor.get_entidad(monitor.getMapa()->casillaOcupada(0))
              ->getSubTipo() == excursionista) {
        monitor.get_entidad(0)->perderPV(0);
        monitor.addMensaje("Ingeniero", "Choco con excursionista");
      } else if (monitor.get_entidad(monitor.getMapa()->casillaOcupada(0))
                     ->getSubTipo() == tecnico) {
        monitor.get_entidad(0)->perderPV(0);
        monitor.addMensaje("Ingeniero", "Choco con tecnico");
      } else if (monitor.get_entidad(monitor.getMapa()->casillaOcupada(0))
                     ->getSubTipo() == vandalo) {
        // Opcion reiniciarse en otro punto del mapa
        /*do
        {
          mix = aleatorio(monitor.getMapa()->getNFils() - 1);
          miy = aleatorio(monitor.getMapa()->getNCols() - 1);

          celdaRand = monitor.getMapa()->getCelda(mix, miy);
        } while (celdaRand == 'P' or celdaRand == 'M' or
        monitor.getMapa()->entidadEnCelda(mix, miy) != '_');

        monitor.get_entidad(0)->resetEntidad();
        monitor.get_entidad(0)->setPosicion(mix, miy);
        monitor.get_entidad(0)->setOrientacion(static_cast<Orientacion>(aleatorio(7)));
        monitor.get_entidad(0)->setHitbox(true);
        monitor.get_entidad(0)->Cogio_Zapatillas(false);*/

        // Opcion simplemente choca contra el vandalo
        monitor.get_entidad(0)->perderPV(0);
        monitor.addMensaje("Ingeniero", "Choco con vandalo");
      }
      salida = false;
    } else // Choca o cae por la diferencia de altura entre casilla inicial y
           // final
    {
      if (difAltJ > 0) // Choca porque la casilla destino está demasiado alta.
      {
        monitor.get_entidad(0)->seAostio();
        monitor.addCollisionMarker(x, y);
        monitor.addMensaje("Ingeniero", "Choco (casilla alta)");
      } else // Se cae porque la casilla destino está demasiado baja.
      {
        monitor.addMensaje("Ingeniero", "Cayo al vacio (desnivel)");
        monitor.get_entidad(0)->resetEntidad();
        monitor.get_entidad(0)->setHitbox(true);
        monitor.finalizarJuego();
        monitor.setMostrarResultados(true);
      }
    }
    break;

  case JUMP:
    posibleElAvance = monitor.getMapa()->EsPosibleCorrer(0, 2);

    if (posibleElAvance != 0) { // No se ha podido avanzar.
      int impactF, impactC;
      if (monitor.getMapa()->EsPosibleCorrer(0, 1) != 0) {
        pair<int, int> c1 = monitor.getMapa()->NCasillasDelante(0, 1);
        impactF = c1.first;
        impactC = c1.second;
      } else {
        impactF = x;
        impactC = y; // Already calculated at distance 2
      }

      switch (posibleElAvance) {
      case 1: // Muro
        monitor.get_entidad(0)->seAostio();
        monitor.addCollisionMarker(impactF, impactC);
        monitor.addMensaje("Ingeniero", "Choco con muro");
        break;

      case 6: // Arbol
        monitor.get_entidad(0)->seAostio();
        monitor.addCollisionMarker(impactF, impactC);
        monitor.addMensaje("Ingeniero", "Choco con arbol");
        break;

      case 2: // Precipicio
        monitor.addMensaje("Ingeniero", "Cayo al precipicio");
        monitor.get_entidad(0)->resetEntidad();
        monitor.get_entidad(0)->setHitbox(true);
        monitor.finalizarJuego();
        monitor.setMostrarResultados(true);
        break;

      case 3: // tecnico
        monitor.get_entidad(0)->seAostio();
        monitor.addCollisionMarker(impactF, impactC);
        monitor.addMensaje("Ingeniero", "Choco con tecnico");
        break;

      case 4: // excursionista
        monitor.get_entidad(0)->seAostio();
        monitor.addCollisionMarker(impactF, impactC);
        monitor.addMensaje("Ingeniero", "Choco con excursionista");
        break;

      case 5: // vandalo
        monitor.get_entidad(0)->seAostio();
        monitor.addCollisionMarker(impactF, impactC);
        monitor.addMensaje("Ingeniero", "Choco con excursionista");
        break;
      }
    } else if (posibleElAvance == 0 and
               ((abs(difAltJ) <= 1) or
                (monitor.get_entidad(0)->Has_Zapatillas() and
                 (abs(difAltJ) <= 2)))) { // Es posible correr
      switch (celdaJ_fin) {
      case 'X': // Puesto Base (Recarga)
        // monitor.get_entidad(0)->increaseBateria(10);
        monitor.get_entidad(0)->setPosicion(x, y);
        salida = true;
        break;
      case 'D': // Casilla Morada (Zapatillas)
        // tomo la zapatilla
        monitor.get_entidad(0)->Cogio_Zapatillas(true);
        // pierdo el bikini
        monitor.get_entidad(0)->setPosicion(x, y);
        salida = true;
        break;
      case 'A': // Agua
      default:
        monitor.get_entidad(0)->setPosicion(x, y);
        salida = true;
        break;
      }
      if (monitor.getLevel() > 1) {
        if (monitor.getLevel() < 4 and
            monitor.get_entidad(0)->isMemberObjetivo(x, y) != -1) {
          // acaba de completar todos los objetivos.
          monitor.addMensaje("Ingeniero", "Objetivo alcanzado");
          // El ingeniero llegó a la casilla objetivo.
          monitor.get_entidad(0)->setCompletoLosObjetivos();
          monitor.get_entidad(0)->incrMisiones();

          monitor.finalizarJuego();
          monitor.setMostrarResultados(true);
        } else if (monitor.getLevel() == 7 and
                   monitor.get_entidad(0)->isMemberObjetivo(x, y) != -1 ) {
          monitor.addMensaje("Ingeniero", "Objetivo alcanzado");
          monitor.get_entidad(0)->setCompletoLosObjetivos();
          monitor.put_active_objetivos(1);
          monitor.get_entidad(0)->anularAlcanzados();
          monitor.get_entidad(0)->incrMisiones();
          monitor.get_entidad(0)->incrPuntuacion(2);
          monitor.get_entidad(1)->AsignarCall_ON(
              false); // Se deja de llamar al tecnico

          for (unsigned int i = 0; i < monitor.numero_entidades(); i++) {
            monitor.get_entidad(i)->setObjetivos(
                monitor.get_active_objetivos());
          }
        } else if (monitor.getLevel() == 7 and
                   monitor.get_entidad(0)->isMemberObjetivo(x, y) != -1 and
                   monitor.CanHeSeesThisCell(
                       1, monitor.get_entidad(0)->getObjFil(0),
                       monitor.get_entidad(0)->getObjCol(0))) {
          monitor.put_active_objetivos(1);
          monitor.get_entidad(0)->anularAlcanzados();
          monitor.get_entidad(0)->incrMisiones();
          monitor.get_entidad(0)->incrPuntuacion(7);
          monitor.get_entidad(1)->AsignarCall_ON(
              false); // Se deja de llamar al tecnico

          for (unsigned int i = 0; i < monitor.numero_entidades(); i++) {
            monitor.get_entidad(i)->setObjetivos(
                monitor.get_active_objetivos());
          }
        }
      }
    } else { // Se puede correr pero el desnivel de altura entre casillas es
             // demasiado grande.
      if (difAltJ > 0) // Choca porque la casilla destino está demasiado alta.
      {
        monitor.get_entidad(0)->seAostio();
        monitor.addCollisionMarker(x, y);
        monitor.addMensaje(
            "Ingeniero ha chocado (por diferencia de altura entre casillas)");
      } else // Se cae porque la casilla destino está demasiado baja.
      {
        monitor.addMensaje(
            "ERROR CRITICO: Ingeniero ha dado un salto al vacío. Demasiada "
            "diferencia de altura entre casillas\n");
        monitor.addMensaje("       FIN DE LA SIMULACION\n");
        monitor.get_entidad(0)->resetEntidad();
        monitor.get_entidad(0)->setHitbox(true);
        monitor.finalizarJuego();
        monitor.setMostrarResultados(true);
      }
    }
    break;

  case TURN_SR:
    monitor.get_entidad(0)->giro45Dch();
    monitor.girarJugadorDerecha(45);
    // monitor.get_entidad(1)->SetActionSent(IDLE);
    if (monitor.getLevel() == 7 and
        monitor.get_entidad(0)->getFil() ==
            monitor.get_entidad(0)->getObjFil(0) and
        monitor.get_entidad(0)->getCol() ==
            monitor.get_entidad(0)->getObjCol(0) and
        monitor.CanHeSeesThisCell(1, monitor.get_entidad(0)->getObjFil(0),
                                  monitor.get_entidad(0)->getObjCol(0))) {
      monitor.put_active_objetivos(1);
      monitor.get_entidad(0)->anularAlcanzados();
      monitor.get_entidad(0)->incrMisiones();
      monitor.get_entidad(0)->incrPuntuacion(7);
      monitor.get_entidad(1)->AsignarCall_ON(
          false); // Se deja de llamar al tecnico

      for (unsigned int i = 0; i < monitor.numero_entidades(); i++) {
        monitor.get_entidad(i)->setObjetivos(monitor.get_active_objetivos());
      }
    }
    salida = false;
    break;

  case TURN_SL:
    monitor.get_entidad(0)->giro45Izq();
    monitor.girarJugadorIzquierda(45);
    // monitor.get_entidad(0)->fixBateria_sig_accion(celdaJ_inicial, accion);
    // monitor.get_entidad(1)->SetActionSent(IDLE);

    if (monitor.getLevel() == 7 and
        monitor.get_entidad(0)->getFil() ==
            monitor.get_entidad(0)->getObjFil(0) and
        monitor.get_entidad(0)->getCol() ==
            monitor.get_entidad(0)->getObjCol(0) and
        monitor.CanHeSeesThisCell(1, monitor.get_entidad(0)->getObjFil(0),
                                  monitor.get_entidad(0)->getObjCol(0))) {
      monitor.put_active_objetivos(1);
      monitor.get_entidad(0)->anularAlcanzados();
      monitor.get_entidad(0)->incrMisiones();
      monitor.get_entidad(0)->incrPuntuacion(7);
      monitor.get_entidad(1)->AsignarCall_ON(
          false); // Se deja de llamar al tecnico

      for (unsigned int i = 0; i < monitor.numero_entidades(); i++) {
        monitor.get_entidad(i)->setObjetivos(monitor.get_active_objetivos());
      }
    }

    salida = true;
    break;

  case PUSH:
    monitor.addMensaje("La acción PUSH no es aplicable al agente ingeniero\n");
    break;

  case INSTALL: {
    bool eng_enfrente = monitor.get_entidad(0)->getEnfrente();
    bool tec_enfrente = monitor.get_entidad(1)->getEnfrente();
    Action tec_action = monitor.get_entidad(1)->getLastAction();

    if (eng_enfrente && tec_enfrente && tec_action == INSTALL) {
      unsigned int fe = monitor.get_entidad(0)->getFil();
      unsigned int ce = monitor.get_entidad(0)->getCol();
      unsigned int ft = monitor.get_entidad(1)->getFil();
      unsigned int ct = monitor.get_entidad(1)->getCol();
      unsigned char he = monitor.getMapa()->alturaEnCelda(fe, ce);
      unsigned char ht = monitor.getMapa()->alturaEnCelda(ft, ct);

      if (he >= (int)ht - 1 && he <= ht) {
        unsigned char bit_e = 0, bit_t = 0;
        if (ft == fe - 1 && ct == ce) {
          bit_e = 1;
          bit_t = 16;
        } // El Técnico está al Norte
        else if (ft == fe + 1 && ct == ce) {
          bit_e = 16;
          bit_t = 1;
        } // El Técnico está al Sur
        else if (ft == fe && ct == ce + 1) {
          bit_e = 4;
          bit_t = 64;
        } // El Técnico está al Este
        else if (ft == fe && ct == ce - 1) {
          bit_e = 64;
          bit_t = 4;
        } // El Técnico está al Oeste

        if (bit_e != 0) {
          monitor.getMapaTuberias()[fe][ce] |= bit_e;
          monitor.getMapaTuberias()[ft][ct] |= bit_t;
          monitor.get_entidad(0)->getComportamiento()->mapaTuberias[fe][ce] |=
              bit_e;
          monitor.get_entidad(0)->getComportamiento()->mapaTuberias[ft][ct] |=
              bit_t;
          monitor.get_entidad(1)->getComportamiento()->mapaTuberias[fe][ce] |=
              bit_e;
          monitor.get_entidad(1)->getComportamiento()->mapaTuberias[ft][ct] |=
              bit_t;

          // Cálculo del impacto ecológico para INSTALL
          int impactoTotal =
              monitor.getCosteEco(INSTALL,
                                  monitor.getMapa()->getCelda(fe, ce)) +
              monitor.getCosteEco(INSTALL, monitor.getMapa()->getCelda(ft, ct));
          monitor.addImpactoEcologico(impactoTotal);

          salida = true;

          if (monitor.getLevel() == 5 or monitor.getLevel() == 6) {
            int startF = monitor.get_entidad(0)->getObjFil(0);
            int startC = monitor.get_entidad(0)->getObjCol(0);
            if (monitor.checkPipeConnection(startF, startC)) {
              if (monitor.getLevel() == 5)
                monitor.addMensaje("¡Nivel 5 completado con Exito! Conexion de tuberias establecida.\n");
              else
                monitor.addMensaje("¡Nivel 6 completado con Exito! Conexion de tuberias establecida.\n");
              monitor.get_entidad(0)->setFin(true);
            }
          }

          monitor.clearFailedAction(fe, ce);
          monitor.clearFailedAction(ft, ct);
        }
      } else {
        monitor.addMensaje("INSTALL: Alturas erroneas");
        monitor.addFailedAction(fe, ce);
        monitor.addFailedAction(ft, ct);
      }
    } else {
      monitor.addMensaje("INSTALL: Falta coord/no enfrentados");
      monitor.addFailedAction(monitor.get_entidad(0)->getFil(),
                              monitor.get_entidad(0)->getCol());
      // También marcar al técnico si al menos están enfrentados pero la acción
      // es errónea
      if (eng_enfrente && tec_enfrente) {
        monitor.addFailedAction(monitor.get_entidad(1)->getFil(),
                                monitor.get_entidad(1)->getCol());
      }
    }
  } break;

  case COME:
    monitor.get_entidad(1)->AsignarCall_ON(true);
    monitor.setDelayCallOn(1);
    monitor.get_entidad(1)->setGoto(monitor.get_entidad(0)->getFil(),
                                    monitor.get_entidad(0)->getCol());
    break;

  case IDLE:
    if (celdaJ_inicial == 'X' and monitor.getLevel() == 7 or
        monitor.getLevel() == 1) { // Casilla Rosa (Recarga)
      monitor.get_entidad(0)->increaseBateria(10);
    }
    // monitor.get_entidad(0)->fixBateria_sig_accion(celdaJ_inicial, accion);
    // monitor.get_entidad(1)->SetActionSent(IDLE);

    if (monitor.getLevel() == 7 and
        monitor.get_entidad(0)->getFil() ==
            monitor.get_entidad(0)->getObjFil(0) and
        monitor.get_entidad(0)->getCol() ==
            monitor.get_entidad(0)->getObjCol(0) and
        monitor.CanHeSeesThisCell(1, monitor.get_entidad(0)->getObjFil(0),
                                  monitor.get_entidad(0)->getObjCol(0))) {
      monitor.put_active_objetivos(1);
      monitor.get_entidad(0)->anularAlcanzados();
      monitor.get_entidad(0)->incrMisiones();
      monitor.get_entidad(0)->incrPuntuacion(7);
      monitor.get_entidad(1)->AsignarCall_ON(
          false); // Se deja de llamar al tecnico
      for (unsigned int i = 0; i < monitor.numero_entidades(); i++) {
        monitor.get_entidad(i)->setObjetivos(monitor.get_active_objetivos());
      }
    }

    salida = true;
    break;

  case DIG: {
    unsigned int f = monitor.get_entidad(0)->getFil();
    unsigned int c = monitor.get_entidad(0)->getCol();
    unsigned char alt = monitor.getMapa()->alturaEnCelda(f, c);
    unsigned char celda = monitor.getMapa()->getCelda(f, c);
    if (celda == 'A') {
      monitor.addMensaje("Ingeniero", "DIG: No aplicable sobre agua");
    } else if (alt > 1) {
      monitor.getMapa()->setAltura(f, c, alt - 1);
      monitor.get_entidad(0)->getComportamiento()->mapaCotas[f][c] = alt - 1;
      // Añadir el impacto ecológico de la acción DIG
      monitor.addImpactoEcologico(
          monitor.getCosteEco(DIG, monitor.getMapa()->getCelda(f, c)));
    }
  }
    salida = true;
    break;

  case RAISE: {
    unsigned int f = monitor.get_entidad(0)->getFil();
    unsigned int c = monitor.get_entidad(0)->getCol();
    unsigned char alt = monitor.getMapa()->alturaEnCelda(f, c);
    unsigned char celda = monitor.getMapa()->getCelda(f, c);
    if (celda == 'A') {
      monitor.addMensaje("Ingeniero", "RAISE: No aplicable sobre agua");
    } else if (alt < 9) {
      monitor.getMapa()->setAltura(f, c, alt + 1);
      monitor.get_entidad(0)->getComportamiento()->mapaCotas[f][c] = alt + 1;

      monitor.addImpactoEcologico(
          monitor.getCosteEco(RAISE, monitor.getMapa()->getCelda(f, c)));
    }
  }
    salida = true;
    break;

    break;
  }

  return salida;
}

bool actuacionTecnico(unsigned char celdaJ_inicial, unsigned char celdaJ_fin,
                      int difAltJ, Action accion, unsigned int x,
                      unsigned int y) {
  int posibleElAvance;
  unsigned int mix, miy;
  unsigned char celdaRand;
  bool salida = false;
  int gasto;
  int error = 0; // 0 = NoError | 1 = NoEnergiaSuficiente | 2 = Colision

  // Primero evaluo si hay energía suficiente para realizar las acciones del
  // ingeniero.

  gasto = monitor.get_entidad(1)->fixBateria_sig_accion_jugador(
      celdaJ_inicial, difAltJ, accion);

  // std::cout << "Accion Ingeniero: " << accion << "   Accion Colaborador: " <<
  // accionColaborador << endl; std::cout << "Gasto Ingeniero: " <<
  // gasto_Ingeniero << "   Gasto Colaborador: " << gasto_colaborador << " Gasto
  // Total: " << gasto << endl;

  if (gasto > monitor.get_entidad(1)->getBateria()) {
    monitor.addMensaje("Tecnico", "Falta energia");
    error = 1;
    monitor.get_entidad(1)->setBateria(0);

    return false;
  }

  switch (accion) {
  case WALK:
    if (monitor.getMapa()->casillaOcupada(1) == -1 and
        abs(difAltJ) <= 1) // Casilla destino desocupada
    {
      switch (celdaJ_fin) {
      case 'M': // Muro
        monitor.get_entidad(1)->seAostio();
        monitor.addCollisionMarker(x, y);
        monitor.addMensaje("Tecnico", "Choco con muro");
        break;
      case 'B': // Arbol
        if (!monitor.get_entidad(1)->Has_Zapatillas()) {
          monitor.get_entidad(1)->seAostio();
          monitor.addCollisionMarker(x, y);
          monitor.addMensaje("Tecnico", "Choco con arbol");
        } else {
          monitor.get_entidad(1)->setPosicion(x, y);
          salida = true;
        }
        break;
      case 'P': // Precipicio
        monitor.addMensaje("Tecnico", "Cayo al precipicio");
        monitor.get_entidad(1)->resetEntidad();
        monitor.get_entidad(1)->setHitbox(true);
        monitor.finalizarJuego();
        monitor.setMostrarResultados(true);
        break;
      case 'X': // Casilla Rosa (Puesto Base)
        monitor.get_entidad(1)->setPosicion(x, y);
        salida = true;
        break;
      case 'D': // Casilla Morada (Zapatillas)
        // tomo la zapatilla
        monitor.get_entidad(1)->Cogio_Zapatillas(true);
        monitor.get_entidad(1)->setPosicion(x, y);
        salida = true;
        break;
      default:
        monitor.get_entidad(1)->setPosicion(x, y);
        salida = true;
        break;
      }
      if (monitor.get_entidad(1)->isMemberObjetivo(x, y) != -1 and
          monitor.get_entidad(1)->allLessOneObjetivosAlcanzados()) {
        if (monitor.getLevel() == 3) {
          // El tecnico llegó a la casilla objetivo.
          monitor.addMensaje("Tecnico", "Objetivo alcanzado");
          monitor.get_entidad(1)->setFin(true);
          monitor.finalizarJuego();
          monitor.setMostrarResultados(true);
        } else if (monitor.getLevel() == 2 or monitor.getLevel() == 7) {
          monitor.addMensaje("Tecnico", "Info: Ing debe llegar al obj");
        }
      }
      // monitor.get_entidad(0)->fixBateria_sig_accion(celdaJ_inicial, accion);
    } else if (abs(difAltJ) > 1) {
      std::cout
          << "Demasiada altura entre las casillas\n"; // Hay demasiada altura
                                                      // entre casillas
      if (difAltJ > 0) // Choca porque la casilla destino está demasiado alta.
      {
        monitor.get_entidad(1)->seAostio();
        monitor.addCollisionMarker(x, y);
        monitor.addMensaje("Tecnico", "Choco (casilla alta)");
      } else // Se cae porque la casilla destino está demasiado baja.
      {
        monitor.addMensaje("Tecnico", "Cayo al vacio (desnivel)");
        monitor.get_entidad(1)->resetEntidad();
        monitor.get_entidad(1)->setHitbox(true);
        monitor.finalizarJuego();
        monitor.setMostrarResultados(true);
      }
    } else if (monitor.getMapa()->casillaOcupada(1) !=
               -1) // Choca contra otro agente
    {
      // Choca contra una entidad
      monitor.get_entidad(1)->seAostio();
      monitor.addCollisionMarker(x, y);
      if (monitor.get_entidad(monitor.getMapa()->casillaOcupada(1))
              ->getSubTipo() == excursionista) {
        monitor.get_entidad(1)->perderPV(0);
        monitor.addMensaje("Tecnico", "Choco con excursionista");
      } else if (monitor.get_entidad(monitor.getMapa()->casillaOcupada(1))
                     ->getTipo() == jugador) {
        monitor.get_entidad(1)->perderPV(0);
        monitor.addMensaje("Tecnico", "Choco con ingeniero");
      } else if (monitor.get_entidad(monitor.getMapa()->casillaOcupada(1))
                     ->getSubTipo() == vandalo) {
        // Opcion reiniciarse en otro punto del mapa
        /*do
        {
          mix = aleatorio(monitor.getMapa()->getNFils() - 1);
          miy = aleatorio(monitor.getMapa()->getNCols() - 1);

          celdaRand = monitor.getMapa()->getCelda(mix, miy);
        } while (celdaRand == 'P' or celdaRand == 'M' or
        monitor.getMapa()->entidadEnCelda(mix, miy) != '_');

        monitor.get_entidad(1)->resetEntidad();
        monitor.get_entidad(1)->setPosicion(mix, miy);
        monitor.get_entidad(1)->setOrientacion(static_cast<Orientacion>(aleatorio(7)));
        monitor.get_entidad(1)->setHitbox(true);
        monitor.get_entidad(1)->Cogio_Zapatillas(false);*/

        // Opcion simplemente choca contra el vandalo
        monitor.get_entidad(1)->perderPV(0);
        monitor.addMensaje("Tecnico", "Choco con vandalo");
      }
      salida = false;
    }

    break;

  case JUMP:
    monitor.addMensaje("Tecnico", "Info: Accion JUMP no aplicable");
    break;

  case TURN_SR:
    monitor.get_entidad(1)->giro45Dch();
    monitor.girarJugadorDerecha(45);
    salida = false;
    break;

  case TURN_SL:
    monitor.get_entidad(1)->giro45Izq();
    monitor.girarJugadorIzquierda(45);
    salida = false;
    break;

  case PUSH:
    monitor.addMensaje("Tecnico", "Info: Accion PUSH no aplicable");
    break;

  case COME:
    monitor.addMensaje("Tecnico", "Info: Accion COME no aplicable");
    break;

    break;

  case INSTALL:
    salida = true;
    break;

  case DIG:
    monitor.addMensaje("Tecnico", "Info: Accion DIG no aplicable");
    break;

  case RAISE:
    monitor.addMensaje("Tecnico", "Info: Accion RAISE no aplicable");
    break;

  case IDLE:
    if (celdaJ_inicial == 'X' and
        (monitor.getLevel() == 1 or
         monitor.getLevel() == 7)) { // Casilla Rosa (Recarga)
      monitor.get_entidad(1)->increaseBateria(10);
    }

    salida = true;
    break;
  }

  return salida;
}

bool actuacionNPC(unsigned int entidad, unsigned char celda, Action accion,
                  unsigned int x, unsigned int y) {
  bool out = false;
  int mix, miy;
  unsigned char celdaRand;

  switch (monitor.get_entidad(entidad)->getSubTipo()) {
  case excursionista: // Acciones para el excursionista
    switch (accion) {
    case WALK:
      if ((celda != 'P' and celda != 'M') and
          monitor.getMapa()->casillaOcupada(entidad) == -1) {
        monitor.get_entidad(entidad)->setPosicion(x, y);
        out = true;
      }
      break;

      /*case TURN_R:
        monitor.get_entidad(entidad)->giroDch();
        out = true;
        break;*/

      /*case TURN_L:
        monitor.get_entidad(entidad)->giroIzq();
        out = true;
        break;*/

    case TURN_SR:
      monitor.get_entidad(entidad)->giro45Dch();
      out = true;
      break;

    case TURN_SL:
      monitor.get_entidad(entidad)->giro45Izq();
      out = true;
      break;
    }
    break;

  case vandalo: // Acciones para el vandalo
    switch (accion) {
    case WALK:
      if ((celda != 'P' and celda != 'M') and
          monitor.getMapa()->casillaOcupada(entidad) == -1) {
        monitor.get_entidad(entidad)->setPosicion(x, y);
        out = true;
      }
      break;

      /*case TURN_R:
        monitor.get_entidad(entidad)->giroDch();
        out = true;
        break;*/

      /*case TURN_L:
        monitor.get_entidad(entidad)->giroIzq();
        out = true;
        break;*/

    case TURN_SR:
      monitor.get_entidad(entidad)->giro45Dch();
      out = true;
      break;

    case TURN_SL:
      monitor.get_entidad(entidad)->giro45Izq();
      out = true;
      break;

    case PUSH: // Esta accion para un vandalo es empujar equivalente a un
               // actPUSH
      monitor.addMensaje("Vandalo empujo a Ingeniero");
      bool esta_ingenierodelante =
          monitor.getMapa()->casillaOcupada(entidad) == 0;
      monitor.get_entidad(0)->seAostio();
      monitor.addCollisionMarker(monitor.get_entidad(0)->getFil(),
                                 monitor.get_entidad(0)->getCol());
      if (esta_ingenierodelante) {
        pair<int, int> casilla =
            monitor.getMapa()->NCasillasDelante(entidad, 2);
        if (monitor.getMapa()->QuienEnCasilla(casilla.first, casilla.second) ==
                -1 and
            monitor.getMapa()->getCelda(casilla.first, casilla.second) !=
                'M' and
            monitor.getMapa()->getCelda(casilla.first, casilla.second) !=
                'P' and
            (monitor.getMapa()->getCelda(casilla.first, casilla.second) != 'B'))
          if (aleatorio(1) == 0) { // Solo ocurre la mitad de las veces que el
                                   // vandalo lo intenta.
            std::cout << "\tEl empujón ha sido efectivo\n";
            monitor.get_entidad(0)->setPosicion(casilla.first, casilla.second);
            monitor.get_entidad(0)->Increment_Empujones();
            monitor.get_entidad(entidad)->giroIzq();
            monitor.get_entidad(entidad)->giro45Izq();
          }
      }
      out = true;
      break;
    }
  }

  return out;
}

pair<int, int> NextCoordenadas(int f, int c, Orientacion brujula) {
  switch (brujula) {
  case norte:
    f = f - 1;
    c = c;
    break;

  case noreste:
    f = f - 1;
    c = c + 1;
    break;

  case este:
    f = f;
    c = c + 1;
    break;

  case sureste:
    f = f + 1;
    c = c + 1;
    break;

  case sur:
    f = f + 1;
    c = c;
    break;

  case suroeste:
    f = f + 1;
    c = c - 1;
    break;

  case oeste:
    f = f;
    c = c - 1;
    break;

  case noroeste:
    f = f - 1;
    c = c - 1;
    break;
  }
  pair<int, int> coordenadas;
  coordenadas.first = f;
  coordenadas.second = c;
  return coordenadas;
}

bool actuacion(unsigned int entidad, Action accion) {
  bool out = false;
  unsigned char celda_inicial, celda_fin, objetivo;
  unsigned int f, c;
  unsigned char altura_inicial, altura_fin;

  f = monitor.get_entidad(entidad)->getFil();
  c = monitor.get_entidad(entidad)->getCol();

  celda_inicial = monitor.getMapa()->getCelda(f, c);
  altura_inicial = monitor.getMapa()->alturaEnCelda(f, c);

  // Calculamos cual es la celda justo frente a la entidad
  pair<unsigned int, unsigned int> coord =
      NextCoordenadas(f, c, monitor.get_entidad(entidad)->getOrientacion());
  if (accion == JUMP and
      monitor.get_entidad(entidad)->getSubTipo() == ingeniero)
    coord = NextCoordenadas(coord.first, coord.second,
                            monitor.get_entidad(entidad)->getOrientacion());
  f = coord.first;
  c = coord.second;
  celda_fin = monitor.getMapa()->getCelda(f, c);
  altura_fin = monitor.getMapa()->alturaEnCelda(f, c);

  // Dependiendo el tipo de la entidad actuamos de una forma u otra

  switch (monitor.get_entidad(entidad)->getSubTipo()) {
  case ingeniero:
    out = actuacionIngeniero(celda_inicial, celda_fin,
                             altura_fin - altura_inicial, accion, f, c);
    break;

  case tecnico:
    out = actuacionTecnico(celda_inicial, celda_fin,
                           altura_fin - altura_inicial, accion, f, c);
    break;

  case excursionista:
    out = actuacionNPC(entidad, celda_fin, accion, f, c);
    break;

  case vandalo:
    out = actuacionNPC(entidad, celda_fin, accion, f, c);
    break;
  }

  return out;
}

void nucleo_motor_juego(MonitorJuego &monitor, int acc) {
  Action accion;
  unsigned char celdaRand;

  // Para borrar despues
  /*  for (int fila = 0; fila < monitor.getMapa()->getNFils(); fila++){
      for (int col = 0; col < monitor.getMapa()->getNCols(); col++){
        if (monitor.getMapa()->getCelda(fila,col) == 'C')
          std::cout << monitor.getMapa()->alturaEnCelda(fila,col);
        else
          std::cout << " ";
      }
      cout << endl;
    }
    cout << endl;

    char ch;*/
  // cin >> ch;

  //====================================

  vector<vector<vector<unsigned char>>> estado;

  estado.clear();

  for (unsigned int i = 0; i < monitor.numero_entidades(); i++) {
    monitor.get_entidad(i)->setEnfrente(false);
  }

  // Ver si están mirándose los dos agentes en casillas adyacentes ortogonales.
  if (monitor.getMapa()->vision(0)[1][2] == 't' and
      monitor.getMapa()->vision(1)[1][2] == 'i') {
    monitor.get_entidad(0)->setEnfrente(true);
    monitor.get_entidad(1)->setEnfrente(true);
  }

  for (unsigned int i = 0; i < monitor.numero_entidades(); i++) {
    estado.push_back(monitor.getMapa()->vision(i));
  }

  // 1. All entities think
  for (unsigned int i = 0; i < monitor.numero_entidades(); i++) {
    Action accion;
    if (i == 0) {
      if (monitor.get_entidad(0)->ready()) {
        monitor.init_casillas_especiales(
            monitor.get_entidad(0)->getFil(), monitor.get_entidad(0)->getCol(),
            monitor.get_entidad(1)->getFil(), monitor.get_entidad(1)->getCol());
        clock_t t0 = clock();
        accion =
            monitor.get_entidad(0)->think(acc, estado[0], monitor.getLevel());
        clock_t t1 = clock();
        monitor.get_entidad(0)->addTiempo(t1 - t0);
        monitor.get_entidad(0)->setLastAction(accion);
      } else {
        monitor.get_entidad(0)->setLastAction(IDLE);
      }
    } else {
      clock_t t0 = clock();
      accion =
          monitor.get_entidad(i)->think(acc, estado[i], monitor.getLevel());
      clock_t t1 = clock();
      monitor.get_entidad(i)->addTiempo(t1 - t0);
      monitor.get_entidad(i)->setLastAction(accion);
    }
  }

  // 2. All entities act
  for (unsigned int i = 0; i < monitor.numero_entidades(); i++) {
    if (i == 0) {
      if (monitor.get_entidad(0)->ready()) {
        actuacion(0, monitor.get_entidad(0)->getLastAction());
      } else {
        monitor.get_entidad(0)->perderPV(0);
      }
    } else {
      actuacion(i, monitor.get_entidad(i)->getLastAction());
    }
  }

  for (unsigned int i = 0; i < monitor.numero_entidades(); i++) {
    monitor.get_entidad(i)->setVision(monitor.getMapa()->vision(i));
  }

  monitor.get_entidad(0)->decBateria_sig_accion();
  monitor.get_entidad(1)->decBateria_sig_accion();
  if (monitor.AnularCallOn()) {
    monitor.get_entidad(1)->AsignarCall_ON(false);
    monitor.decCallON();
  } else {
    monitor.decCallON();
    cout << "Se mantiene activo el COME al Tech\n";
  }

  monitor.decPasos();

  if (acc != -1) {
    // sleep(monitor.getRetardo() / 10);
  }

  // Verificar si se dan las condiciones de salida con éxito de la simulacion
  if (!monitor.get_entidad(0)->fin() and !monitor.get_entidad(1)->fin()) {
    // Reviso si se dan las condiciones de éxito en el nivel 0
    switch (monitor.getLevel()) {
    case 0: // Nivel 0 -> Reactivo los dos en un puesto base
      if (monitor.getMapa()->getCelda(monitor.get_entidad(0)->getFil(),
                                      monitor.get_entidad(0)->getCol()) ==
              'U' and
          !monitor.getRecargaPisadaIngeniero()) {
        monitor.setRecargaPisadaIngeniero(true);
        monitor.setRecargaAbandonadaIngeniero(false);
        monitor.addMensaje("Ingeniero", "Planta Belkanita alcanzada");
      }
      if (monitor.getMapa()->getCelda(monitor.get_entidad(1)->getFil(),
                                      monitor.get_entidad(1)->getCol()) ==
              'U' and
          !monitor.getRecargaPisadaTecnico()) {
        monitor.setRecargaPisadaTecnico(true);
        monitor.setRecargaAbandonadaTecnico(false);
        monitor.addMensaje("Tecnico", "Planta Belkanita alcanzada");
      }
      if (monitor.getMapa()->getCelda(monitor.get_entidad(0)->getFil(),
                                      monitor.get_entidad(0)->getCol()) !=
              'U' and
          monitor.getRecargaPisadaIngeniero() and
          !monitor.getRecargaAbandonadaIngeniero()) {
        monitor.setRecargaAbandonadaIngeniero(true);
        monitor.setRecargaPisadaIngeniero(false);
        monitor.addMensaje("Ingeniero", "Planta Belkanita abandonada");
      }
      if (monitor.getMapa()->getCelda(monitor.get_entidad(1)->getFil(),
                                      monitor.get_entidad(1)->getCol()) !=
              'U' and
          monitor.getRecargaPisadaTecnico() and
          !monitor.getRecargaAbandonadaTecnico()) {
        monitor.setRecargaAbandonadaTecnico(true);
        monitor.setRecargaPisadaTecnico(false);
        monitor.addMensaje("Tecnico", "Planta Belkanita abandonada");
      }

      if (monitor.getMapa()->getCelda(monitor.get_entidad(0)->getFil(),
                                      monitor.get_entidad(0)->getCol()) !=
              'U' and
          monitor.getMapa()->getCelda(monitor.get_entidad(0)->getFil(),
                                      monitor.get_entidad(0)->getCol()) !=
              'D' and
          monitor.getMapa()->getCelda(monitor.get_entidad(0)->getFil(),
                                      monitor.get_entidad(0)->getCol()) !=
              'C' and
          !monitor.getCaminoAbandonadoIngeniero()) {
        monitor.setCaminoAbandonadoIngeniero(true);
        monitor.addMensaje("Ingeniero", "Aviso: Fuera de camino");
      }

      if (monitor.getMapa()->getCelda(monitor.get_entidad(1)->getFil(),
                                      monitor.get_entidad(1)->getCol()) !=
              'U' and
          monitor.getMapa()->getCelda(monitor.get_entidad(1)->getFil(),
                                      monitor.get_entidad(1)->getCol()) !=
              'D' and
          monitor.getMapa()->getCelda(monitor.get_entidad(1)->getFil(),
                                      monitor.get_entidad(1)->getCol()) !=
              'C' and
          (!monitor.get_entidad(1)->Has_Zapatillas() or
           monitor.getMapa()->getCelda(monitor.get_entidad(1)->getFil(),
                                       monitor.get_entidad(1)->getCol()) !=
               'B') and
          !monitor.getCaminoAbandonadoTecnico()) {
        monitor.setCaminoAbandonadoTecnico(true);
        monitor.addMensaje("Tecnico", "Aviso: Fuera de camino");
      }

      if (monitor.getMapa()->getCelda(monitor.get_entidad(0)->getFil(),
                                      monitor.get_entidad(0)->getCol()) ==
              'U' and
          monitor.getMapa()->getCelda(monitor.get_entidad(1)->getFil(),
                                      monitor.get_entidad(1)->getCol()) ==
              'U') {
        // monitor.get_entidad(0)->setFin(true);
        // monitor.get_entidad(1)->setFin(true);
        monitor.addMensaje("Sistema", "Mision completada: Puesto Base");
        if (monitor.getRecargaPisadaIngeniero()) {
          monitor.addMensaje("Ingeniero", "Llego a puesto base");
        }
        if (monitor.getRecargaPisadaTecnico()) {
          monitor.addMensaje("Tecnico", "Llego a puesto base");
        }
        if (monitor.getRecargaAbandonadaIngeniero()) {
          monitor.addMensaje("Ingeniero", "Abandono puesto base");
        }
        if (monitor.getRecargaAbandonadaTecnico()) {
          monitor.addMensaje("Tecnico", "Abandono puesto base");
        }

        if (monitor.getCaminoAbandonadoIngeniero()) {
          monitor.addMensaje("Ingeniero", "Aviso: Fuera de camino");
        }
        if (monitor.getCaminoAbandonadoTecnico()) {
          monitor.addMensaje("Tecnico", "Aviso: Fuera de camino");
        }

        monitor.finalizarJuego();
        monitor.setMostrarResultados(true);
      }
      break;
    case 1: // Nivel 1 -> Termina cuando se agotan los recursos de ciclos,
            // tiempo o energía.
      break;
    case 2: // Nivel 2 -> Termina cuando el agente ingeniero llega a la casilla
            // objetivo.
      break;
    case 3: // Nivel 3 -> Termina cuando el agente tecnico llega a la casilla
            // objetivo.
      break;
    case 4: // Nivel 4 -> Termina cuando el agente ingeniero construye un plan
            // de tuberías.
      if (monitor.checkLevel4()) {
        monitor.addMensaje("Sistema", "¡Nivel 4 completado con Exito!");
        monitor.get_entidad(0)->setFin(true);
        monitor.finalizarJuego();
        monitor.setMostrarResultados(true);
      }
      break;
    case 5: // Nivel 5 -> Termina cuando se agontan los recursos de ciclos,
            // tiempo o energia.
    case 6: // Nivel 6 -> Mismo comportamiento de finalización que Nivel 5.
      if (monitor.finJuego()) {
        monitor.setMostrarResultados(true);
      }
      break;
    }
  }
}

bool lanzar_motor_juego(int &colisiones, int acc) {
  bool out = false;

  if (monitor.continuarBelkan()) {
    if (monitor.jugar()) {
      if ((monitor.getPasos() != 0) and (!monitor.finJuego())) {
        nucleo_motor_juego(monitor, acc);
      }
    }

    if (monitor.mostrarResultados()) {
      stringstream ss;
      if (monitor.getLevel() == 5 or monitor.getLevel() == 6) {
        if (monitor.get_entidad(0)->fin() or monitor.get_entidad(1)->fin()) {
          ss << "Nivel " << monitor.getLevel() << " completado con Exito! Conexion de tuberias establecida.";
        } else {
          ss << "Nivel " << monitor.getLevel() << " NO completado.";
        }
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
        ss.str("");
        ss << "Instantes consumidos: "
           << monitor.getInstantesInicial() -
                  monitor.get_entidad(0)->getInstantesPendientes();
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
        ss.str("");
        ss << "Consumo Energia Ingeniero: "
           << monitor.getEnergiaInicial() -
                  monitor.get_entidad(0)->getBateria();
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
        ss.str("");
        ss << "Consumo Energia Tecnico:   "
           << monitor.getEnergiaInicial() -
                  monitor.get_entidad(1)->getBateria();
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;

        ss.str("");
        ss << "Impacto Ecologico: " << monitor.getImpactoEcologico();
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;

        int longitud_red = 0;
        auto &mapaTuberias = monitor.getMapaTuberias();
        for (int i = 0; i < (int)mapaTuberias.size(); i++) {
          for (int j = 0; j < (int)mapaTuberias[i].size(); j++) {
            if (mapaTuberias[i][j] != 0) {
              longitud_red++;
            }
          }
        }
        ss.str("");
        ss << "Longitud red tuberias: " << longitud_red;
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
      } else if (monitor.getLevel() == 4) {
        if (monitor.get_entidad(0)->fin()) {
          ss << "Nivel 4 completado con Exito!";
        } else {
          ss << "Nivel 4 NO completado.";
        }
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
        ss.str("");
        ss << "Longitud red tuberias: "
           << monitor.get_entidad(0)->getCanalizacionPlan().size();
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;

        int simulated_impact = 0;
        ListaCasillasPlan plan = monitor.get_entidad(0)->getCanalizacionPlan();
        if (!plan.empty()) {
          auto it = plan.begin();
          unsigned char celda = monitor.getMapa()->getCelda(it->fil, it->col);
          if (it->op == -1)
            simulated_impact += monitor.getCosteEco(DIG, celda);
          if (it->op == 1)
            simulated_impact += monitor.getCosteEco(RAISE, celda);

          auto prev = it;
          ++it;
          for (; it != plan.end(); ++it) {
            celda = monitor.getMapa()->getCelda(it->fil, it->col);
            if (it->op == -1)
              simulated_impact += monitor.getCosteEco(DIG, celda);
            if (it->op == 1)
              simulated_impact += monitor.getCosteEco(RAISE, celda);

            unsigned char celda_prev =
                monitor.getMapa()->getCelda(prev->fil, prev->col);
            simulated_impact += monitor.getCosteEco(INSTALL, celda_prev);
            simulated_impact += monitor.getCosteEco(INSTALL, celda);
            prev = it;
          }
        }
        ss.str("");
        ss << "Impacto Ecologico: " << simulated_impact;
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
      } else if (monitor.getLevel() == 3) {
        if (monitor.get_entidad(1)->fin()) {
          ss << "Nivel 3 completado con Exito!";
        } else {
          ss << "Nivel 3 NO completado.";
        }
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
        ss.str("");
        ss << "Longitud camino Tecnico: "
           << monitor.getInstantesInicial() -
                  monitor.get_entidad(1)->getInstantesPendientes();
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
        ss.str("");
        ss << "Consumo Energia Tecnico: "
           << monitor.getEnergiaInicial() -
                  monitor.get_entidad(1)->getBateria();
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
        ss.str("");
        ss << "Instantes consumidos: "
           << monitor.getInstantesInicial() -
                  monitor.get_entidad(0)->getInstantesPendientes();
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
        ss.str("");
        ss << "Tiempo: "
           << (monitor.get_entidad(0)->getTiempo() +
               monitor.get_entidad(1)->getTiempo()) /
                  CLOCKS_PER_SEC
           << "s";
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
      } else if (monitor.getLevel() == 1) {
        if (monitor.get_entidad(0)->vivo() and monitor.get_entidad(1)->vivo() and
            monitor.get_entidad(0)->getBateria() > 0 and monitor.get_entidad(1)->getBateria() > 0 and
            monitor.getImpactoEcologico() < monitor.getMaxImpacto()) {
          ss << "Nivel 1 completado con Exito!";
        } else {
          ss << "Nivel 1 NO completado.";
        }
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
        ss.str("");
        ss << "Descubierto Senderos: "
           << monitor.CoincidenciaConElMapaCaminosYSenderos() << "%";
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
        ss.str("");
        ss << "Descubierto Mapa: " << monitor.CoincidenciaConElMapa() << "%";
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
        ss.str("");
        ss << "Consumo Energia Ingeniero: "
           << monitor.getEnergiaInicial() -
                  monitor.get_entidad(0)->getBateria();
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
        ss.str("");
        ss << "Consumo Energia Tecnico:   "
           << monitor.getEnergiaInicial() -
                  monitor.get_entidad(1)->getBateria();
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
        ss.str("");
        ss << "Tiempo: "
           << (monitor.get_entidad(0)->getTiempo() +
               monitor.get_entidad(1)->getTiempo()) /
                  CLOCKS_PER_SEC
           << "s";
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
      } else if (monitor.getLevel() == 0) {
        if (monitor.get_entidad(0)->vivo() and monitor.get_entidad(1)->vivo() and
            monitor.get_entidad(0)->getBateria() > 0 and monitor.get_entidad(1)->getBateria() > 0) {
          ss << "Nivel 0 completado con Exito!";
        } else {
          ss << "Nivel 0 NO completado.";
        }
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
        ss.str("");
        ss << "Energia Ingeniero: " << monitor.get_entidad(0)->getBateria();
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
        ss.str("");
        ss << "Energia Tecnico:   " << monitor.get_entidad(1)->getBateria();
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
        ss.str("");
        ss << "Instantes consumidos: "
           << monitor.getInstantesInicial() -
                  monitor.get_entidad(0)->getInstantesPendientes();
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
        ss.str("");
        ss << "Tiempo: "
           << (monitor.get_entidad(0)->getTiempo() +
               monitor.get_entidad(1)->getTiempo()) /
                  CLOCKS_PER_SEC
           << "s";
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
      } else if (monitor.getLevel() == 2) {
        if (monitor.get_entidad(0)->fin()) {
          ss << "Nivel 2 completado con Exito!";
        } else {
          ss << "Nivel 2 NO completado.";
        }
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
      } else {
        if (monitor.get_entidad(0)->fin() or monitor.get_entidad(1)->fin()) {
          ss << "Nivel " << monitor.getLevel() << " completado con Exito!";
        } else {
          ss << "Nivel " << monitor.getLevel() << " NO completado.";
        }
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
        ss.str("");
        ss << "Instantes restantes: "
           << monitor.get_entidad(0)->getInstantesPendientes();
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
        ss.str("");
        ss << "Tiempo: "
           << (monitor.get_entidad(0)->getTiempo() +
               monitor.get_entidad(1)->getTiempo()) /
                  CLOCKS_PER_SEC
           << "s";
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
        ss.str("");
        ss << "Energia Ingeniero: " << monitor.get_entidad(0)->getBateria();
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
        ss.str("");
        ss << "Energia Tecnico:   " << monitor.get_entidad(1)->getBateria();
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
        ss.str("");
        ss << "Colisiones: "
           << monitor.get_entidad(0)->getColisiones() +
                  monitor.get_entidad(1)->getColisiones();
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
        ss.str("");
        ss << "Descubierto Senderos: "
           << monitor.CoincidenciaConElMapaCaminosYSenderos() << "%";
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
        ss.str("");
        ss << "Descubierto Mapa: " << monitor.CoincidenciaConElMapa() << "%";
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
        ss.str("");
        ss << "Objetivos: (" << monitor.get_entidad(0)->getMisiones() << ") "
           << monitor.get_entidad(0)->getPuntuacion();
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
        ss.str("");
        ss << "Impacto Ecologico: " << monitor.getImpactoEcologico();
        monitor.addMensaje("Sistema", ss.str());
        cout << ss.str() << endl;
      }

      monitor.setMostrarResultados(false);
      out = true;
    }
  }
  return out;
}

void lanzar_motor_juego2(MonitorJuego &monitor) {

  // monitor.get_entidad(0)->setObjetivo(monitor.getObjFil(),
  // monitor.getObjCol());

  while (!monitor.finJuego() && monitor.jugar()) {
    nucleo_motor_juego(monitor, -1);
  }

  if (monitor.mostrarResultados() and (monitor.getLevel() == 0)) {
    stringstream ss;
    if (monitor.get_entidad(0)->vivo() and monitor.get_entidad(1)->vivo() and
        monitor.get_entidad(0)->getBateria() > 0 and monitor.get_entidad(1)->getBateria() > 0) {
      ss << "Nivel 0 completado con Exito!";
    } else {
      ss << "Nivel 0 NO completado.";
    }
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    ss.str("");
    ss << "Energia Ingeniero: " << monitor.get_entidad(0)->getBateria();
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    ss.str("");
    ss << "Energia Tecnico:   " << monitor.get_entidad(1)->getBateria();
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    ss.str("");
    ss << "Instantes consumidos: "
       << monitor.getInstantesInicial() -
              monitor.get_entidad(0)->getInstantesPendientes();
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    ss.str("");
    ss << "Tiempo: "
       << (monitor.get_entidad(0)->getTiempo() +
           monitor.get_entidad(1)->getTiempo()) /
              CLOCKS_PER_SEC
       << "s";
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    monitor.setMostrarResultados(false);
  } else if (monitor.mostrarResultados() and (monitor.getLevel() == 1)) {
    stringstream ss;
    if (monitor.get_entidad(0)->vivo() and monitor.get_entidad(1)->vivo() and
        monitor.get_entidad(0)->getBateria() > 0 and monitor.get_entidad(1)->getBateria() > 0 and
        monitor.getImpactoEcologico() < monitor.getMaxImpacto()) {
      ss << "Nivel 1 completado con Exito!";
    } else {
      ss << "Nivel 1 NO completado.";
    }
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    ss.str("");
    ss << "Descubierto Senderos: "
       << monitor.CoincidenciaConElMapaCaminosYSenderos() << "%";
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    ss.str("");
    ss << "Descubierto Mapa: " << monitor.CoincidenciaConElMapa() << "%";
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    ss.str("");
    ss << "Consumo Energia Ingeniero: "
       << monitor.getEnergiaInicial() - monitor.get_entidad(0)->getBateria();
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    ss.str("");
    ss << "Consumo Energia Tecnico:   "
       << monitor.getEnergiaInicial() - monitor.get_entidad(1)->getBateria();
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    ss.str("");
    ss << "Tiempo: "
       << (monitor.get_entidad(0)->getTiempo() +
           monitor.get_entidad(1)->getTiempo()) /
              CLOCKS_PER_SEC
       << "s";
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    monitor.setMostrarResultados(false);
  } else if (monitor.mostrarResultados() and (monitor.getLevel() == 2)) {
    stringstream ss;
    if (monitor.get_entidad(0)->fin()) {
      ss << "Nivel 2 completado con Exito!";
    } else {
      ss << "Nivel 2 NO completado.";
    }
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    ss.str("");
    ss << "Longitud camino Ingeniero: "
       << monitor.getInstantesInicial() -
              monitor.get_entidad(0)->getInstantesPendientes();
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    ss.str("");
    ss << "Consumo Energia Ingeniero: "
       << monitor.getEnergiaInicial() - monitor.get_entidad(0)->getBateria();
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    ss.str("");
    ss << "Instantes consumidos: "
       << monitor.getInstantesInicial() -
              monitor.get_entidad(0)->getInstantesPendientes();
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    ss.str("");
    ss << "Tiempo: "
       << (monitor.get_entidad(0)->getTiempo() +
           monitor.get_entidad(1)->getTiempo()) /
              CLOCKS_PER_SEC
       << "s";
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    monitor.setMostrarResultados(false);
  } else if (monitor.mostrarResultados() and monitor.getLevel() == 3) {
    stringstream ss;
    if (monitor.get_entidad(1)->fin()) {
      ss << "Nivel 3 completado con Exito!";
    } else {
      ss << "Nivel 3 NO completado.";
    }
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    ss.str("");
    ss << "Longitud camino Tecnico: "
       << monitor.getInstantesInicial() -
              monitor.get_entidad(1)->getInstantesPendientes();
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    ss.str("");
    ss << "Consumo Energia Tecnico: "
       << monitor.getEnergiaInicial() - monitor.get_entidad(1)->getBateria();
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    ss.str("");
    ss << "Instantes consumidos: "
       << monitor.getInstantesInicial() -
              monitor.get_entidad(0)->getInstantesPendientes();
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    ss.str("");
    ss << "Tiempo: "
       << (monitor.get_entidad(0)->getTiempo() +
           monitor.get_entidad(1)->getTiempo()) /
              CLOCKS_PER_SEC
       << "s";
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    monitor.setMostrarResultados(false);
  } else if (monitor.mostrarResultados() and monitor.getLevel() == 4) {
    stringstream ss;
    if (monitor.get_entidad(0)->fin()) {
      ss << "Nivel 4 completado con Exito!";
    } else {
      ss << "Nivel 4 NO completado.";
    }
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    ss.str("");
    ss << "Longitud red tuberias: "
       << monitor.get_entidad(0)->getCanalizacionPlan().size();
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;

    int simulated_impact = 0;
    ListaCasillasPlan plan = monitor.get_entidad(0)->getCanalizacionPlan();
    if (!plan.empty()) {
      auto it = plan.begin();
      unsigned char celda = monitor.getMapa()->getCelda(it->fil, it->col);
      if (it->op == -1)
        simulated_impact += monitor.getCosteEco(DIG, celda);
      if (it->op == 1)
        simulated_impact += monitor.getCosteEco(RAISE, celda);

      auto prev = it;
      ++it;
      for (; it != plan.end(); ++it) {
        celda = monitor.getMapa()->getCelda(it->fil, it->col);
        if (it->op == -1)
          simulated_impact += monitor.getCosteEco(DIG, celda);
        if (it->op == 1)
          simulated_impact += monitor.getCosteEco(RAISE, celda);

        unsigned char celda_prev =
            monitor.getMapa()->getCelda(prev->fil, prev->col);
        simulated_impact += monitor.getCosteEco(INSTALL, celda_prev);
        simulated_impact += monitor.getCosteEco(INSTALL, celda);
        prev = it;
      }
    }
    ss.str("");
    ss << "Impacto Ecologico: " << simulated_impact;
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    monitor.setMostrarResultados(false);
  } else if (monitor.mostrarResultados() and (monitor.getLevel() == 5 or monitor.getLevel() == 6)) {
    stringstream ss;
    if (monitor.get_entidad(0)->fin() or monitor.get_entidad(1)->fin()) {
      ss << "Nivel " << monitor.getLevel() << " completado con Exito! Conexion de tuberias establecida.";
    } else {
      ss << "Nivel " << monitor.getLevel() << " NO completado.";
    }
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    ss.str("");
    ss << "Instantes consumidos: "
       << monitor.getInstantesInicial() -
              monitor.get_entidad(0)->getInstantesPendientes();
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    ss.str("");
    ss << "Consumo Energia Ingeniero: "
       << monitor.getEnergiaInicial() - monitor.get_entidad(0)->getBateria();
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    ss.str("");
    ss << "Consumo Energia Tecnico:   "
       << monitor.getEnergiaInicial() - monitor.get_entidad(1)->getBateria();
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;

    ss.str("");
    ss << "Impacto Ecologico: " << monitor.getImpactoEcologico();
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;

    int longitud_red_2 = 0;
    auto &mapaTuberias_2 = monitor.getMapaTuberias();
    for (int i = 0; i < (int)mapaTuberias_2.size(); i++) {
      for (int j = 0; j < (int)mapaTuberias_2[i].size(); j++) {
        if (mapaTuberias_2[i][j] != 0) {
          longitud_red_2++;
        }
      }
    }
    ss.str("");
    ss << "Longitud red tuberias: " << longitud_red_2;
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    monitor.setMostrarResultados(false);
  } else if (monitor.mostrarResultados()) {
    stringstream ss;
    if (monitor.get_entidad(0)->fin() or monitor.get_entidad(1)->fin()) {
      ss << "Nivel " << monitor.getLevel() << " completado con Exito!";
    } else {
      ss << "Nivel " << monitor.getLevel() << " NO completado.";
    }
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    ss.str("");
    ss << "Instantes restantes: "
       << monitor.get_entidad(0)->getInstantesPendientes();
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    ss.str("");
    ss << "Tiempo: "
       << (monitor.get_entidad(0)->getTiempo() +
           monitor.get_entidad(1)->getTiempo()) /
              CLOCKS_PER_SEC
       << "s";
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    ss.str("");
    ss << "Energia Ingeniero: " << monitor.get_entidad(0)->getBateria();
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    ss.str("");
    ss << "Energia Tecnico:   " << monitor.get_entidad(1)->getBateria();
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    ss.str("");
    ss << "Colisiones: "
       << monitor.get_entidad(0)->getColisiones() +
              monitor.get_entidad(1)->getColisiones();
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    ss.str("");
    ss << "Descubierto Senderos: "
       << monitor.CoincidenciaConElMapaCaminosYSenderos() << "%";
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    ss.str("");
    ss << "Descubierto Mapa: " << monitor.CoincidenciaConElMapa() << "%";
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    ss.str("");
    ss << "Objetivos: (" << monitor.get_entidad(0)->getMisiones() << ") "
       << monitor.get_entidad(0)->getPuntuacion();
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;
    ss.str("");
    ss << "Impacto Ecologico: " << monitor.getImpactoEcologico();
    monitor.addMensaje("Sistema", ss.str());
    cout << ss.str() << endl;

    monitor.setMostrarResultados(false);
  }
}
