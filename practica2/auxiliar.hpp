#ifndef COMPORTAMIENTOAUXILIAR_H
#define COMPORTAMIENTOAUXILIAR_H

#include <chrono>
#include <time.h>
#include <thread>
#include <list>
#include <cmath>
#include <vector>
#include <iostream>
#include <string>
#include <set>
#include <list>
#include <queue>
#include <algorithm>
#include <climits>

#include "comportamientos/comportamiento.hpp"

// Variables de Estado para el nivel E
struct EstadoA
{
  int f;
  int c;
  int brujula;
  bool zapatillas;
  bool operator==(const EstadoA &st) const
  {
    return f == st.f && c == st.c && brujula == st.brujula && zapatillas == st.zapatillas;
  }
  // operador !=
  bool operator!=(const EstadoA &st) const
  {
    return !(*this == st);
  }
};

struct NodoA
{
  EstadoA estado;
  vector<Action> secuencia;
  int coste;
  int prioridad; // Coste + Heuristica para A*
  bool operator>(const NodoA &node) const
  {
    return coste > node.coste;
  }

  bool operator==(const NodoA &node) const
  {
    return estado == node.estado;
  }
  bool operator<(const NodoA &node) const
  {
    if (estado.f < node.estado.f)
      return true;
    else if (estado.f == node.estado.f and estado.c < node.estado.c)
      return true;
    else if (estado.f == node.estado.f and estado.c == node.estado.c and estado.brujula < node.estado.brujula)
      return true;
    else if (estado.f == node.estado.f and estado.c == node.estado.c and estado.brujula == node.estado.brujula and estado.zapatillas < node.estado.zapatillas)
      return true;
    else
      return false;
  }
};

struct CasillaDescubrimiento_Auxiliar
{
  int f, c;
  int numInterrogantes;

  // Comparador inverso porque priority_queue es un max-heap por defecto
  bool operator<(const CasillaDescubrimiento_Auxiliar &other) const
  {
    return numInterrogantes < other.numInterrogantes;
  }
};

class ComportamientoAuxiliar : public Comportamiento
{
public:
  ComportamientoAuxiliar(unsigned int size = 0) : Comportamiento(size)
  {
    // Inicializar Variables de Estado Niveles 0,1,4
    last_action = IDLE;
    tiene_zapatillas = false;
    giro45Izq = 0;

    // Mis variables del nivel 0
    acciones_Seguidas = 0;
    contador_Acciones = 0;
    contador = 0;
    hayAgua = false;
    vecesAgua = 0;
    haySenderos = false;
    vecesSenderos = 0;

    // Mis variables del nivel 1
    visitadosBool.resize(mapaResultado.size(), vector<bool>(mapaResultado[0].size(), false));
    vecesVisitadas.resize(mapaResultado.size(), vector<int>(mapaResultado[0].size(), 0));
    evitarColisionInicial = false;

    // Variables de Estado para el nivel 4
    llamadaAlAuxiliar = false;
    vueltasReconocimiento = false;
    hayPlan = false;
    tiene_zapatillas = false;
    esPrimerTurno = true;
    visitadosCasillasInterrogacion.resize(mapaResultado.size(), vector<bool>(mapaResultado[0].size(), false));
    contadorAgua = 0;
    lastPosition = make_pair(-1, -1);
    contadorPosicionesIguales = 0;
  }
  ComportamientoAuxiliar(vector<vector<unsigned char>> mapaR, vector<vector<unsigned char>> mapaC) : Comportamiento(mapaR, mapaC)
  {
    // Inicializar Variables de Estado Niveles 2,3
    hayPlan = false;
    tiene_zapatillas = false;
  }
  ComportamientoAuxiliar(const ComportamientoAuxiliar &comport) : Comportamiento(comport) {}
  ~ComportamientoAuxiliar() {}

  Action think(Sensores sensores);

  int interact(Action accion, int valor);

  Action ComportamientoAuxiliarNivel_0(Sensores sensores);
  Action ComportamientoAuxiliarNivel_1(Sensores sensores);
  Action ComportamientoAuxiliarNivel_2(Sensores sensores);
  Action ComportamientoAuxiliarNivel_3(Sensores sensores);
  Action ComportamientoAuxiliarNivel_4(Sensores sensores);

  Action ComportamientoAuxiliarNivel_E(Sensores sensores);

  char ViablePorAltura(char casilla, int dif);

  int VeoCasillaInteresante(char i, char c, char d);

  // Funciones nivel 0
  void ContarAgua(Sensores sensores);
  void ContarSenderos(Sensores sensores);
  string DescripcionTerreno(char terreno);

  // Funciones nivel 1
  int VeoCasillaInteresanteNivel1(char i, char c, char d, bool zap);
  bool CasillVisitadamasde10veces(int f, int c);
  bool escasillavalida_lvl1(Sensores sensores, int n);
  vector<Action> obtenerPosiblesMovimientos(const Sensores &sensores, int filaActual, int colActual);
  bool evitarColisionAuxiliar(const Sensores &sensores, Action &accion, int &giro45Izq);

  // Funciones de nivel E
  list<Action> AnchuraAuxiliar(const EstadoA &inicio, const EstadoA &final, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);
  EstadoA applyA(Action accion, const EstadoA &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);
  bool Find(const NodoA &st, const list<NodoA> &lista);
  void VisualizaPlan(const EstadoA &st, vector<Action> &plan);
  EstadoA NextCasillaAuxiliar(const EstadoA &st);
  bool CasillaAccesibleAuxiliar(const EstadoA &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);
  list<Action> AnchuraAuxiliar_V2(const EstadoA &inicio, const EstadoA &final, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);

  // Funciones de nivel 2
  vector<Action> AEstrellaAuxiliar(const EstadoA &origen, const EstadoA &destino, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);
  int CosteBaseTerreno(unsigned char casilla, Action accion);
  int CosteCambioAltura(int altura_origen, int altura_destino, unsigned char casilla, Action accion);
  void PintaPlan(vector<Action> &plan, bool zap);

  // Funciones de nivel 4
  Action comportamientoReactivo(Sensores sensores, int &giro45Izq);
  priority_queue<CasillaDescubrimiento_Auxiliar> cola_con_casillas_con_interrogacion(Sensores sensores);
  vector<Action> AEstrellaAuxiliar_lvl4(const EstadoA &origen, EstadoA &destino, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura, bool esparaRecargar);
  bool casillValidad_lvl4(Sensores sensores, int pos, bool tiene_zapatillas);

private:
  // Variables de Estado
  Action last_action;    // Ultima Accion Realizada
  bool tiene_zapatillas; // Tiene Zapatillas
  int giro45Izq;         // Giro 45 Izquierda

  // Mis variables del nivel 0
  int acciones_Seguidas;
  int contador_Acciones;
  static const int MAX_ACCIONES = 1200;
  int contador = 0;
  bool hayAgua;
  int vecesAgua;
  bool haySenderos;
  int vecesSenderos;

  // Mis variables del nivel 1
  vector<vector<int>> vecesVisitadas;
  vector<vector<bool>> visitadosBool;
  bool evitarColisionInicial;
  int contador_iteraciones = 0;

  // Variables de Estado para el nivel E
  vector<Action> plan;
  bool hayPlan;

  // Variables de Estado para el nivel 4
  bool llamadaAlAuxiliar;
  bool vueltasReconocimiento;
  bool esPrimerTurno;
  bool recargaEnergia;
  bool estoyRecargando;
  set<pair<int, int>> listaPuestosRecarga;
  vector<vector<bool>> visitadosCasillasInterrogacion;
  int contadorAgua;

  pair<int, int> lastPosition;
  int contadorPosicionesIguales;
};

#endif
