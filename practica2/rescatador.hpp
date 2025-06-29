#ifndef COMPORTAMIENTORESCATADOR_H
#define COMPORTAMIENTORESCATADOR_H

#include <chrono>
#include <time.h>
#include <thread>
#include <utility>
#include <string>
#include <cmath>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <map>
#include <set>
#include <list>
#include <climits>

#include "comportamientos/comportamiento.hpp"

// VERSIÓN FINAL

struct EstadoR
{
  int f;
  int c;
  int brujula;
  bool zapatillas;

  bool operator==(const EstadoR &st) const
  {
    return f == st.f && c == st.c && brujula == st.brujula && zapatillas == st.zapatillas;
  }
  bool operator<(const EstadoR &otro) const
  {
    if (f != otro.f)
      return f < otro.f;
    if (c != otro.c)
      return c < otro.c;
    if (brujula != otro.brujula)
      return brujula < otro.brujula;
    return zapatillas < otro.zapatillas;
  }
};

// Este lo usamos para el nivel 2.
struct NodoR
{
  EstadoR estado;
  list<Action> secuencia;
  int coste = 0; // añadimos coste para Dijkstra

  bool operator==(const NodoR &node) const
  {
    return estado == node.estado;
  }

  bool operator<(const NodoR &node) const
  {
    if (estado.f < node.estado.f)
      return true;
    else if (estado.f == node.estado.f && estado.c < node.estado.c)
      return true;
    else if (estado.f == node.estado.f && estado.c == node.estado.c && estado.brujula < node.estado.brujula)
      return true;
    else if (estado.f == node.estado.f && estado.c == node.estado.c && estado.brujula == node.estado.brujula && estado.zapatillas < node.estado.zapatillas)
      return true;
    else
      return false;
  }
  bool operator>(const NodoR &other) const
  {
    return this->coste > other.coste;
  }
};

struct CasillaDescubrimiento
{
  int f, c;
  int numInterrogantes;

  // Comparador inverso porque priority_queue es un max-heap por defecto
  bool operator<(const CasillaDescubrimiento &other) const
  {
    return numInterrogantes < other.numInterrogantes;
  }
};

class ComportamientoRescatador : public Comportamiento
{

public:
  ComportamientoRescatador(unsigned int size = 0) : Comportamiento(size)
  {
    // Inicializar Variables de Estado Niveles 0,1,4
    last_action = IDLE;
    tiene_zapatillas = false;
    giro45Izq = 0;

    // Mis variables del nivel 0
    acciones_Seguidas = 0;
    contador_Acciones = 0;
    contador = 0;
    visitados.resize(mapaResultado.size(), vector<int>(mapaResultado[0].size(), 0));
    visitadosBool.resize(mapaResultado.size(), vector<bool>(mapaResultado[0].size(), false));
    hayAgua = false;
    vecesAgua = 0;
    contadorVisitasConsecutivas = 0;

    // Variables del nivel 4
    hayPlan = false;
    tiene_zapatillas = false;
    puntuacion_equipo = 0;
    energiaInicial = 0;
    energiaFinal = 0;
    misionAceptada = false;
    estoyenX = false;
    objetivoActual = make_pair(-1, -1);
    recargaEnergia = false;
    estoyRecargando = false;
    visitadosCasillasInterrogacion.resize(mapaResultado.size(), vector<bool>(mapaResultado[0].size(), false));
    planDijkstra = false;
    posicionAuxiliar = make_pair(-1, -1);
    lastPosition = make_pair(-1, -1);
    contadorMismaPosicion = 0;
  }
  ComportamientoRescatador(vector<vector<unsigned char>> mapaR, vector<vector<unsigned char>> mapaC) : Comportamiento(mapaR, mapaC)
  {
    // Inicializar Variables de Estado Niveles 2,3
    hayPlan = false;
    tiene_zapatillas = false;
  }
  ComportamientoRescatador(const ComportamientoRescatador &comport) : Comportamiento(comport) {}
  ~ComportamientoRescatador() {}

  Action think(Sensores sensores);

  int interact(Action accion, int valor);

  Action ComportamientoRescatadorNivel_0(Sensores sensores);
  Action ComportamientoRescatadorNivel_1(Sensores sensores);
  Action ComportamientoRescatadorNivel_2(Sensores sensores);
  Action ComportamientoRescatadorNivel_3(Sensores sensores);
  Action ComportamientoRescatadorNivel_4(Sensores sensores);

  Action ComportamientoRescatadorNivel_E(Sensores sensores);

  int VeoCasillaInteresante(char i, char c, char d, bool zap);
  char ViablePorAltura(char casilla, int dif, bool zap);
  Action EscaneoInicial(Sensores sensores);

  // Funciones de nivel 0
  void ContarAgua(Sensores sensores);

  // Funciones de nivel 1
  int VeoCasillaInteresanteNivel1(char i, char c, char d, bool zap);
  bool hayXalrededor(Sensores sensores, int &posI);
  bool CasillVisitadamasde10veces(int f, int c);
  bool escasillavalida_lvl1(Sensores sensores, int n);
  bool casillaVisitada(int f, int c);
  vector<Action> obtenerPosiblesMovimientos(const Sensores &sensores, int filaActual, int colActual);
  bool evitarColisionAuxiliar(const Sensores &sensores, Action &accion, int &giro45Izq);
  Action PriorizarCaminosDeInterrogacion(Sensores sensores, int fila, int col);

  // Funciones para el nivel E
  void VisualizaPlan(const EstadoR &st, const list<Action> &plan);
  void PintaPlan(const list<Action> &plan, bool zap);
  bool Find(const NodoR &st, const list<NodoR> &lista);
  EstadoR NextCasillaRescatador(const EstadoR &st);
  bool CasillaAccesibleRescatador(const EstadoR &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);
  EstadoR applyR(Action accion, const EstadoR &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);
  list<Action> AnchuraRescatador(const EstadoR &origen, const EstadoR &destino, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);

  // Funciones para el nivel 2
  int CosteBaseTerreno(unsigned char casilla, Action accion);
  int CosteCambioAltura(int altura_origen, int altura_destino, unsigned char casilla, Action accion);
  list<Action> DijkstraRescatador(const EstadoR &inicio, const EstadoR &final, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);

  // Funciones para el nivel 4
  bool casillValidad_lvl4(Sensores sensores, int pos);
  int hayVandaloExcursionista(Sensores sensores, int pos);
  bool decirleAlAuxiliar(Sensores sensores);
  bool casillaNoTransitable(Sensores sensores, int pos);
  priority_queue<CasillaDescubrimiento> cola_con_casillas_con_interrogacion(Sensores sensores);
  list<Action> AEstrellaRescatador(const EstadoR &origen, const EstadoR &destino, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);

private:
  // Variables de Estado
  Action last_action;    // Ultima Accion Realizada
  bool tiene_zapatillas; // Tiene Zapatillas
  int giro45Izq;         // Giro 45 Izquierda

  // Mis variables del nivel 0
  int acciones_Seguidas;
  int contador_Acciones;
  int contador;
  static const int MAX_ACCIONES = 700;
  vector<vector<int>> visitados;
  vector<vector<bool>> visitadosBool;
  bool hayAgua;
  int vecesAgua;

  // Mis variables del nivel 1
  int contadorVisitasConsecutivas;

  // Mis variables del nivel E
  list<Action> plan;
  bool hayPlan;

  // Variables del nivel 4
  int puntuacion_equipo;
  int energiaInicial;
  int energiaFinal;
  bool misionAceptada;
  bool estoyenX;
  pair<int, int> objetivoActual;
  bool recargaEnergia;
  bool estoyRecargando;
  set<pair<int, int>> listaPuestosRecarga;
  vector<vector<bool>> visitadosCasillasInterrogacion;
  bool planDijkstra;
  pair<int, int> posicionAuxiliar;

  pair<int, int> lastPosition;
  int contadorMismaPosicion;
};

#endif
