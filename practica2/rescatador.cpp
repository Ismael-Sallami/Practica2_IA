#include "../Comportamientos_Jugador/rescatador.hpp"
#include "motorlib/util.h"
#include <iostream>
using namespace std;

/*
CABE DESTACAR QUE HAY FUNCIONES EXTRAS QUE SE IMPLEMENTARON PERO QUE AL FINAL NO SE USARON
*/

// VERSION FINAL

Action ComportamientoRescatador::think(Sensores sensores)
{
  Action accion = IDLE;

  switch (sensores.nivel)
  {
  case 0:
    accion = ComportamientoRescatadorNivel_0(sensores);
    break;
  case 1:
    accion = ComportamientoRescatadorNivel_1(sensores);
    break;
  case 2:
    accion = ComportamientoRescatadorNivel_2(sensores);
    break;
  case 3:
    accion = ComportamientoRescatadorNivel_3(sensores);
    // accion = ComportamientoRescatadorNivel_E(sensores);
    break;
  case 4:
    accion = ComportamientoRescatadorNivel_4(sensores);
    break;
  }

  return accion;
}

int ComportamientoRescatador::interact(Action accion, int valor)
{
  return 0;
}

// ════════════════════════════════════════════════════════════════════════════
//                              FUNCIONES NIVEL 0
// ════════════════════════════════════════════════════════════════════════════

// Esta función devuelve un valor según la casilla que ve el agente., luego en el switch de más abajo ajustamos el comportamiento.
int ComportamientoRescatador::VeoCasillaInteresante(char i, char c, char d, bool zap)
{
  if (c == 'X')
    return 2; // si en centro es un camino transitable que me devuelva 2 que es que debo de ir por ele centro y así con las demás.
  else if (i == 'X')
    return 1;
  else if (d == 'X')
    return 3;
  else if (!zap)
  {
    if (c == 'D')
      return 2;
    else if (i == 'D')
      return 1;
    else if (d == 'D')
      return 3;
  }
  if (c == 'C')
    return 2;
  else if (i == 'C')
    return 1;
  else if (d == 'C')
    return 3;
  else
    return 0;
}

// Esta función devuelve si la casilla es viable por altura o no, si no lo es devuelve P.
char ComportamientoRescatador::ViablePorAltura(char casilla, int dif, bool zap)
{
  if (abs(dif) <= 1 or (zap and abs(dif) <= 2))
    return casilla;
  else
    return 'P';
}

// Esta función devuelve una descripción del terreno según el carácter que recibe como parámetro.
string DescripcionTerreno(char terreno)
{
  switch (terreno)
  {
  case 'B':
    return "Bosque (no transitable en general)";
  case 'T':
    return "Matorral (terreno abrupto, transitable pero costoso)";
  case 'A':
    return "Agua (transitable para algunos agentes, consume mucha energía)";
  case 'P':
    return "Preci picio (intransitable, caer supone el final de la simulación)";
  case 'C':
    return "Camino (fácil de caminar, conecta puestos )";
  case 'S':
    return "Sendero (transitable, consume más energía que caminos)";
  case 'M':
    return "Obstáculo (intransitable, antiguas construcciones o piedras grandes)";
  case 'D':
    return "Zapatillas (mejora habilidades del agente)";
  case 'X':
    return "Puesto  (objetivo)";
  case '?':
    return "Desconocido (parte del mundo aún no explorada)";
  default:
    return "Terreno no identificado";
  }
}

// ════════════════════════════════════════════════════════════════════════════

// ════════════════════════════════════════════════════════════════════════════
//                             NIVEL 0
// ════════════════════════════════════════════════════════════════════════════

Action ComportamientoRescatador::ComportamientoRescatadorNivel_0(Sensores sensores)
{
  // Vemos numerosos // cout, pero que ayudan a depurar el código.
  // cout << "#################RESCATADOR-INICIO#################" << endl;

  Action accion;
  visitados[sensores.posF][sensores.posC]++;
  visitadosBool[sensores.posF][sensores.posC] = true;
  contador++;
  // cout << "Contador: " << contador << endl;

  // pinta el valor de sensor superficie y la de los adyacentes
  // cout << "Terreno actual: " << DescripcionTerreno(sensores.superficie[0]) << endl;
  // cout << "Terreno delante: " << DescripcionTerreno(sensores.superficie[2]) << endl;
  // cout << "Terreno derecha: " << DescripcionTerreno(sensores.superficie[3]) << endl;
  // cout << "Terreno izquierda: " << DescripcionTerreno(sensores.superficie[1]) << endl;
  // cout << "Sensores superficie en la casilla 6 :" << sensores.superficie[6] << endl;
  // El comportamiento de seguir un camino hasta encontrar un puesto base.
  // Actualización de variables de estado
  // SituarSensorEnMapaR(mapaResultado, mapaCotas, sensores);
  if (visitados[sensores.posF][sensores.posC] > 1500)
  {
    // cout << "Entra en el max acciones y el valor de la casilla es: " << visitados[sensores.posF][sensores.posC] << endl;
    // devolvemos un random entre 0,1,2 y si la casilla es transitable la pasa.
    return TURN_SR;
  }
  if (sensores.superficie[0] == 'D')
    tiene_zapatillas = true;

  // pintamos las dimensiones del mapa usando mapaResultado
  // cout << "Dimensiones del mapa: " << mapaResultado.size() << "x" << mapaResultado[0].size() << endl;

  // Definición del comportamiento
  if (sensores.superficie[0] == 'X')
  { // Llegué al Objetivo
    return IDLE;
  }
  else if (giro45Izq != 0)
  { // Estoy haciendo un TURN_SL
    accion = TURN_SR;
    giro45Izq--;
  }
  else if (contador > 2200)
  { // este if se usa para aumentar la aleatoriedad, y de esta manera mejorar el comporamiento reactivo del código y conseguir la meta.
    if (sensores.superficie[2] == 'X' || sensores.superficie[6] == 'X')
      return WALK;
    if (contador < 2500)
    {
      if ((sensores.superficie[3] == 'X' || sensores.superficie[3] == 'C') && sensores.agentes[3] != 'a')
      {
        return TURN_SR;
      }
      else if (sensores.superficie[2] == 'X' || sensores.superficie[2] == 'C' || sensores.superficie[6] == 'X')
      {
        return WALK;
      }
      else
        return TURN_L;
    }
    else
    {
      if ((sensores.superficie[1] == 'X' || sensores.superficie[1] == 'C') && sensores.agentes[1] != 'a')
      {
        return TURN_L;
      }
      else if (sensores.superficie[2] == 'X' || sensores.superficie[2] == 'C' || sensores.superficie[6] == 'X')
      {
        return WALK;
      }
      else
        return TURN_SR;
    }
  }
  else
  {

    char i = ViablePorAltura(sensores.superficie[1], sensores.cota[1] - sensores.cota[0], tiene_zapatillas);
    char c = ViablePorAltura(sensores.superficie[2], sensores.cota[2] - sensores.cota[0], tiene_zapatillas);
    char d = ViablePorAltura(sensores.superficie[3], sensores.cota[3] - sensores.cota[0], tiene_zapatillas);

    int pos = VeoCasillaInteresante(i, c, d, tiene_zapatillas);

    switch (pos)
    {

    case 2: // Vamos a avanzar hacia delante
      // cout << "Entra en case 2" << endl;
      if (sensores.agentes[2] == 'a' || sensores.agentes[6] == 'a' || sensores.agentes[12] == 'a')
      {
        // cout << "Evito posible colisión con Auxiliar (cerca o delante+1)" << endl;
        accion = TURN_SR;
        giro45Izq = 3;
      }
      else
      {
        // Verificar opciones transitables, de nuevo al ser el caso que más se devuelve en la mayoría de los casos, aumentamos la aleatoriedad.
        bool delante = (sensores.superficie[2] == 'C' || sensores.superficie[2] == 'X');
        bool derecha = (sensores.superficie[3] == 'C' || sensores.superficie[3] == 'X');
        bool izquierda = (sensores.superficie[1] == 'C' || sensores.superficie[1] == 'X');

        // Si hay múltiples opciones, elegir una aleatoria
        if (delante && derecha && izquierda)
        {
          int opcion = rand() % 3; // Generar un número aleatorio entre 0 y 2
          if (opcion == 0)
          {
            accion = WALK; // Avanzar
          }
          else if (opcion == 1)
          {
            accion = TURN_SR; // Girar a la izquierda
            giro45Izq = 6;
          }
          else
          {
            accion = TURN_SR; // Girar a la derecha
          }
        }
        else if (delante && derecha)
        {
          int opcion = rand() % 2; // Generar un número aleatorio entre 0 y 1
          if (opcion == 0)
          {
            accion = WALK; // Avanzar
          }
          else
          {
            accion = TURN_SR; // Girar a la derecha
          }
        }
        else if (delante && izquierda)
        {
          int opcion = rand() % 2; // Generar un número aleatorio entre 0 y 1
          if (opcion == 0)
          {
            accion = WALK; // Avanzar
          }
          else
          {
            accion = TURN_SR; // Girar a la izquierda
            giro45Izq = 6;
          }
        }
        else
        {
          contador_Acciones++;
          if (contador_Acciones > MAX_ACCIONES)
          {
            // cout << "Entra en el if donde no hay caminos transitables Y MUCHAS OPCIONES" << endl;
            // calcula un random entre 0,1,2 y si la casilla es transitable la pasa.
            int opcion = rand() % 3; // Generar un número aleatorio entre 0 y 2
            if (opcion == 0)
            {
              return TURN_L;
            }
            else if (opcion == 1)
            {
              return accion = TURN_SR;
            }
            else if (opcion == 2)
            {
              return accion = WALK;
            }
          }
          else
          {
            return WALK;
          }
        }
      }
      break;
    case 1:
      // cout << "Entra en case 1" << endl;
      giro45Izq = 1;
      accion = TURN_L;
      break;
    case 3:
      // cout << "Entra en case 3" << endl;
      accion = TURN_SR;
      break;
    case 0:
      // cout << "Entra en case 0" << endl;
      // comprobamos si los superficie 6 no son viable por altura
      char correr = ViablePorAltura(sensores.superficie[6], sensores.cota[6] - sensores.cota[0], tiene_zapatillas);
      if (correr == 'P')
      {
        // cout << "Entra en el if de la casilla 6" << endl;
        return TURN_L;
      }

      // contador ++, esto se habia añadido para aumentar la aleatoriedad, pero no es necesario.

      // si la casilla de supeficies en 2 y 6 es C o D que devuelva RUN, esto es para evitar que el rescatador se quede atascado en un bucle y así usar el RUN, además de que hay mapas específicos que lo requieren.
      if ((sensores.superficie[2] == 'C' || sensores.superficie[2] == 'D' || sensores.superficie[6] == 'C' || sensores.superficie[6] == 'D') &&
          (sensores.superficie[2] != 'M' && sensores.superficie[6] != 'M'))
      {
        // cout << "Entra en el if de la casilla 2 y 6" << endl;
        return RUN;
      }
      accion = TURN_L;
      break;
    }
  }

  if (last_action == accion)
  {
    acciones_Seguidas++; // aumentamos el contador de acciones seguidas, ya que hay ciertos mapas que se quedan atascados en un bucle y no avanzan, como es el caso del ajedrez.
  }
  else if (acciones_Seguidas > 0)
  {
    acciones_Seguidas--;
  }

  // Imprimimos el contador
  // cout << "Contador: " << contador << endl;
  // cout << "#################RESCATADOR-FIN################# con terreno" << sensores.superficie[0] << endl;

  if (sensores.energia == 2512)
    return TURN_L;
  // Devolver la siguiente acción a hacer
  last_action = accion;
  return accion;
}

// ════════════════════════════════════════════════════════════════════════════

// ════════════════════════════════════════════════════════════════════════════
//                              FUNCIONES NIVEL 1
// ════════════════════════════════════════════════════════════════════════════

int ComportamientoRescatador::VeoCasillaInteresanteNivel1(char i, char c, char d, bool zap)
{
  if (!zap)
  {
    if (c == 'D')
      return 2; // Zapatillas si no las tiene
    else if (i == 'D')
      return 1;
    else if (d == 'D')
      return 3;
  }
  if (c == 'C' || c == 'S')
    return 2; // Caminos transitables
  else if (i == 'C' || i == 'S')
    return 1;
  else if (d == 'C' || d == 'S')
    return 3;
  else
    return 0; // Ninguna casilla interesante

  if (c == 'X')
    return 2; 
  else if (i == 'X')
    return 1;
  else if (d == 'X')
    return 3;
}

pair<int, int> rotarR(int f, int c, int rumbo)
{
  switch (rumbo)
  {
  case 0:
    return {f, c}; // Norte
  case 1:
    return {(f - c), (f + c)}; // Noreste
  case 2:
    return {c, -f}; // Este
  case 3:
    return {(-f - c), (c - f)}; // Sureste
  case 4:
    return {-f, -c}; // Sur
  case 5:
    return {(-c + f), (-f - c)}; // Suroeste
  case 6:
    return {-c, f}; // Oeste
  case 7:
    return {(c + f), (-c + f)}; // Noroeste
  default:
    return {f, c};
  }
}

void SituarSensorEnMapaR(vector<vector<unsigned char>> &mapaTerreno, vector<vector<unsigned char>> &mapaAlturas, const Sensores &sensores)
{
  pair<int, int> desplazamientosEste[16] = {{0, 0}, {-1, 1}, {0, 1}, {1, 1}, {-2, 2}, {-1, 2}, {0, 2}, {1, 2}, {2, 2}, {-3, 3}, {-2, 3}, {-1, 3}, {0, 3}, {1, 3}, {2, 3}, {3, 3}};
  pair<int, int> desplazamientosOeste[16] = {{0, 0}, {1, -1}, {0, -1}, {-1, -1}, {2, -2}, {1, -2}, {0, -2}, {-1, -2}, {-2, -2}, {3, -3}, {2, -3}, {1, -3}, {0, -3}, {-1, -3}, {-2, -3}, {-3, -3}};
  pair<int, int> desplazamientosNorte[16] = {{0, 0}, {-1, -1}, {-1, 0}, {-1, 1}, {-2, -2}, {-2, -1}, {-2, 0}, {-2, 1}, {-2, 2}, {-3, -3}, {-3, -2}, {-3, -1}, {-3, 0}, {-3, 1}, {-3, 2}, {-3, 3}};
  pair<int, int> desplazamientosSur[16] = {{0, 0}, {1, 1}, {1, 0}, {1, -1}, {2, 2}, {2, 1}, {2, 0}, {2, -1}, {2, -2}, {3, 3}, {3, 2}, {3, 1}, {3, 0}, {3, -1}, {3, -2}, {3, -3}};
  pair<int, int> desplazamientosSuroeste[16] = {{0, 0}, {1, 0}, {1, -1}, {0, -1}, {2, 0}, {2, -1}, {2, -2}, {1, -2}, {0, -2}, {3, 0}, {3, -1}, {3, -2}, {3, -3}, {2, -3}, {1, -3}, {0, -3}};
  pair<int, int> desplazamientosSureste[16] = {{0, 0}, {0, 1}, {1, 1}, {1, 0}, {0, 2}, {1, 2}, {2, 2}, {2, 1}, {2, 0}, {0, 3}, {1, 3}, {2, 3}, {3, 3}, {3, 2}, {3, 1}, {3, 0}};
  pair<int, int> desplazamientosNoreste[16] = {{0, 0}, {-1, 0}, {-1, 1}, {0, 1}, {-2, 0}, {-2, 1}, {-2, 2}, {-1, 2}, {0, 2}, {-3, 0}, {-3, 1}, {-3, 2}, {-3, 3}, {-2, 3}, {-1, 3}, {0, 3}};
  pair<int, int> desplazamientosNoroeste[16] = {{0, 0}, {0, -1}, {-1, -1}, {-1, 0}, {0, -2}, {-1, -2}, {-2, -2}, {-2, -1}, {-2, 0}, {0, -3}, {-1, -3}, {-2, -3}, {-3, -3}, {-3, -2}, {-3, -1}, {-3, 0}};

  pair<int, int> desplazamientosSeleccionados[16];
  switch (sensores.rumbo)
  {
  case norte:
    for (int i = 0; i < 16; i++)
    {
      desplazamientosSeleccionados[i] = desplazamientosNorte[i];
    }
    break;
  case sur:
    for (int i = 0; i < 16; i++)
    {
      desplazamientosSeleccionados[i] = desplazamientosSur[i];
    }
    break;
  case este:
    for (int i = 0; i < 16; i++)
    {
      desplazamientosSeleccionados[i] = desplazamientosEste[i];
    }
    break;
  case oeste:
    for (int i = 0; i < 16; i++)
    {
      desplazamientosSeleccionados[i] = desplazamientosOeste[i];
    }
    break;
  case noroeste:
    for (int i = 0; i < 16; i++)
    {
      desplazamientosSeleccionados[i] = desplazamientosNoroeste[i];
    }
    break;
  case noreste:
    for (int i = 0; i < 16; i++)
    {
      desplazamientosSeleccionados[i] = desplazamientosNoreste[i];
    }
    break;
  case sureste:
    for (int i = 0; i < 16; i++)
    {
      desplazamientosSeleccionados[i] = desplazamientosSureste[i];
    }
    break;
  case suroeste:
    for (int i = 0; i < 16; i++)
    {
      desplazamientosSeleccionados[i] = desplazamientosSuroeste[i];
    }
    break;
  }

  for (int i = 0; i < 16; i++)
  {
    mapaTerreno[sensores.posF + desplazamientosSeleccionados[i].first][sensores.posC + desplazamientosSeleccionados[i].second] = sensores.superficie[i];
    mapaAlturas[sensores.posF + desplazamientosSeleccionados[i].first][sensores.posC + desplazamientosSeleccionados[i].second] = sensores.cota[i];
  }
}

bool ComportamientoRescatador::hayXalrededor(Sensores sensores, int &posI)
{
  char i = ViablePorAltura(sensores.superficie[1], sensores.cota[1] - sensores.cota[0], tiene_zapatillas);
  char c = ViablePorAltura(sensores.superficie[2], sensores.cota[2] - sensores.cota[0], tiene_zapatillas);
  char d = ViablePorAltura(sensores.superficie[3], sensores.cota[3] - sensores.cota[0], tiene_zapatillas);

  if (c == 'X')
  {
    posI = 2; // Centro
    return true;
  }
  else if (i == 'X')
  {
    posI = 1; // Izquierda
    return true;
  }
  else if (d == 'X')
  {
    posI = 3; // Derecha
    return true;
  }

  return false;
}

bool ComportamientoRescatador::CasillVisitadamasde10veces(int f, int c)
{
  if (visitados[f][c] > 6)
  {
    return true;
  }
  return false;
}

bool ComportamientoRescatador::escasillavalida_lvl1(Sensores sensores, int n)
{
  if ((sensores.superficie[n] == 'C' || sensores.superficie[n] == 'D' || sensores.superficie[n] == 'X' || sensores.superficie[n] == 'S') && (ViablePorAltura(sensores.superficie[n], sensores.cota[n] - sensores.cota[0], tiene_zapatillas) != 'P'))
  {
    return true;
  }
  return false;
}

bool ComportamientoRescatador::casillaVisitada(int f, int c)
{
  if (visitadosBool[f][c])
  {
    return true;
  }
  else
    return false;
}

vector<Action> ComportamientoRescatador::obtenerPosiblesMovimientos(const Sensores &sensores, int filaActual, int colActual)
{
  vector<Action> posiblesMovimientos;

  if (sensores.rumbo == norte)
  {
    if (filaActual - 1 >= 0 && escasillavalida_lvl1(sensores, 2) && !visitadosBool[filaActual - 1][colActual])
    {
      posiblesMovimientos.push_back(WALK);
    }
    else if (filaActual - 1 >= 0 && colActual - 1 >= 0 && escasillavalida_lvl1(sensores, 1) && !visitadosBool[filaActual - 1][colActual - 1])
    {
      giro45Izq = 1;
      posiblesMovimientos.push_back(TURN_L);
    }
    else if (filaActual - 1 >= 0 && colActual + 1 < visitadosBool[0].size() && escasillavalida_lvl1(sensores, 3) && !visitadosBool[filaActual - 1][colActual + 1])
    {
      posiblesMovimientos.push_back(TURN_SR);
    }
  }
  else if (sensores.rumbo == noreste)
  {
    if (filaActual - 1 >= 0 && colActual + 1 < visitadosBool[0].size() && escasillavalida_lvl1(sensores, 2) && !visitadosBool[filaActual - 1][colActual + 1])
    {
      posiblesMovimientos.push_back(WALK);
    }
    else if (filaActual - 1 >= 0 && escasillavalida_lvl1(sensores, 1) && !visitadosBool[filaActual - 1][colActual])
    {
      giro45Izq = 1;
      posiblesMovimientos.push_back(TURN_L);
    }
    else if (colActual + 1 < visitadosBool[0].size() && escasillavalida_lvl1(sensores, 3) && !visitadosBool[filaActual][colActual + 1])
    {
      posiblesMovimientos.push_back(TURN_SR);
    }
  }
  else if (sensores.rumbo == este)
  {
    if (colActual + 1 < visitadosBool[0].size() && escasillavalida_lvl1(sensores, 2) && !visitadosBool[filaActual][colActual + 1])
    {
      posiblesMovimientos.push_back(WALK);
    }
    else if (filaActual - 1 >= 0 && colActual + 1 < visitadosBool[0].size() && escasillavalida_lvl1(sensores, 1) && !visitadosBool[filaActual - 1][colActual + 1])
    {
      giro45Izq = 1;
      posiblesMovimientos.push_back(TURN_L);
    }
    else if (filaActual + 1 < visitadosBool.size() && colActual + 1 < visitadosBool[0].size() && escasillavalida_lvl1(sensores, 3) && !visitadosBool[filaActual + 1][colActual + 1])
    {
      posiblesMovimientos.push_back(TURN_SR);
    }
  }
  else if (sensores.rumbo == sureste)
  {
    if (filaActual + 1 < visitadosBool.size() && colActual + 1 < visitadosBool[0].size() && escasillavalida_lvl1(sensores, 2) && !visitadosBool[filaActual + 1][colActual + 1])
    {
      posiblesMovimientos.push_back(WALK);
    }
    else if (colActual + 1 < visitadosBool[0].size() && escasillavalida_lvl1(sensores, 1) && !visitadosBool[filaActual][colActual + 1])
    {
      giro45Izq = 1;
      posiblesMovimientos.push_back(TURN_L);
    }
    else if (filaActual + 1 < visitadosBool.size() && escasillavalida_lvl1(sensores, 3) && !visitadosBool[filaActual + 1][colActual])
    {
      posiblesMovimientos.push_back(TURN_SR);
    }
  }
  else if (sensores.rumbo == sur)
  {
    if (filaActual + 1 < visitadosBool.size() && escasillavalida_lvl1(sensores, 2) && !visitadosBool[filaActual + 1][colActual])
    {
      posiblesMovimientos.push_back(WALK);
    }
    else if (filaActual + 1 < visitadosBool.size() && colActual + 1 < visitadosBool[0].size() && escasillavalida_lvl1(sensores, 1) && !visitadosBool[filaActual + 1][colActual + 1])
    {
      giro45Izq = 1;
      posiblesMovimientos.push_back(TURN_L);
    }
    else if (filaActual + 1 < visitadosBool.size() && colActual - 1 >= 0 && escasillavalida_lvl1(sensores, 3) && !visitadosBool[filaActual + 1][colActual - 1])
    {
      posiblesMovimientos.push_back(TURN_SR);
    }
  }
  else if (sensores.rumbo == suroeste)
  {
    if (filaActual + 1 < visitadosBool.size() && colActual - 1 >= 0 && escasillavalida_lvl1(sensores, 2) && !visitadosBool[filaActual + 1][colActual - 1])
    {
      posiblesMovimientos.push_back(WALK);
    }
    else if (filaActual + 1 < visitadosBool.size() && escasillavalida_lvl1(sensores, 1) && !visitadosBool[filaActual + 1][colActual])
    {
      giro45Izq = 1;
      posiblesMovimientos.push_back(TURN_L);
    }
    else if (colActual - 1 >= 0 && escasillavalida_lvl1(sensores, 3) && !visitadosBool[filaActual][colActual - 1])
    {
      posiblesMovimientos.push_back(TURN_SR);
    }
  }
  else if (sensores.rumbo == oeste)
  {
    if (colActual - 1 >= 0 && escasillavalida_lvl1(sensores, 2) && !visitadosBool[filaActual][colActual - 1])
    {
      posiblesMovimientos.push_back(WALK);
    }
    else if (filaActual + 1 < visitadosBool.size() && colActual - 1 >= 0 && escasillavalida_lvl1(sensores, 1) && !visitadosBool[filaActual + 1][colActual - 1])
    {
      giro45Izq = 1;
      posiblesMovimientos.push_back(TURN_L);
    }
    else if (filaActual - 1 >= 0 && colActual - 1 >= 0 && escasillavalida_lvl1(sensores, 3) && !visitadosBool[filaActual - 1][colActual - 1])
    {
      posiblesMovimientos.push_back(TURN_SR);
    }
  }
  else if (sensores.rumbo == noroeste)
  {
    if (filaActual - 1 >= 0 && colActual - 1 >= 0 && escasillavalida_lvl1(sensores, 2) && !visitadosBool[filaActual - 1][colActual - 1])
    {
      posiblesMovimientos.push_back(WALK);
    }
    else if (colActual - 1 >= 0 && escasillavalida_lvl1(sensores, 1) && !visitadosBool[filaActual][colActual - 1])
    {
      giro45Izq = 1;
      posiblesMovimientos.push_back(TURN_L);
    }
    else if (filaActual - 1 >= 0 && escasillavalida_lvl1(sensores, 3) && !visitadosBool[filaActual - 1][colActual])
    {
      posiblesMovimientos.push_back(TURN_SR);
    }
  }

  return posiblesMovimientos;
}

string traducirAccion(Action accion)
{
  switch (accion)
  {
  case WALK:
    return "camina hacia delante";
  case TURN_L:
    return "gira a la izquierda";
  case TURN_SR:
    return "gira suavemente a la derecha";
  case IDLE:
    return "se queda quieto";
  case RUN:
    return "corre hacia delante";
  default:
    return "acción desconocida";
  }
}

bool ComportamientoRescatador::evitarColisionAuxiliar(const Sensores &sensores, Action &accion, int &giro45Izq)
{
  if (sensores.agentes[2] == 'a' || sensores.agentes[6] == 'a' || sensores.agentes[12] == 'a')
  {
    // cout << "Evito posible colisión con Auxiliar (cerca o delante+1)" << endl;
    accion = TURN_L;
    return true;
  }
  return false;
}

Action ComportamientoRescatador::PriorizarCaminosDeInterrogacion(Sensores sensores, int fila, int col)
{
  vector<Action> posiblesMovimientos = obtenerPosiblesMovimientos(sensores, fila, col);
  for (const auto &movimiento : posiblesMovimientos)
  {
    if (movimiento == WALK && sensores.superficie[2] == '?')
    {
      return movimiento;
    }
    else if (movimiento == TURN_L && sensores.superficie[1] == '?')
    {
      return movimiento;
    }
    else if (movimiento == TURN_SR && sensores.superficie[3] == '?')
    {
      return movimiento;
    }
    else
      return IDLE;
  }
  return IDLE;
}

// ════════════════════════════════════════════════════════════════════════════

// ════════════════════════════════════════════════════════════════════════════
//                               NIVEL 1
// ════════════════════════════════════════════════════════════════════════════

Action ComportamientoRescatador::ComportamientoRescatadorNivel_1(Sensores sensores)
{
  // Vemos numerosos // cout, pero que ayudan a depurar el código.
  // cout << "#################RESCATADOR-INICIO#################" << endl;
  Action accion;
  visitados[sensores.posF][sensores.posC]++;
  visitadosBool[sensores.posF][sensores.posC] = true;
  contador++;
  int posI = 0;
  int fila = sensores.posF;
  int col = sensores.posC;
  vector<Action> posiblesMovimientos = obtenerPosiblesMovimientos(sensores, fila, col);

  bool evitarColisionInicial = evitarColisionAuxiliar(sensores, accion, giro45Izq);

  if (evitarColisionInicial)
  {
    // cout << "Evitar colisión con Auxiliar Al inicio" << endl;
    return accion;
  }
  else
  {
    // cout << "No evitar colisión con Auxiliar AL inicio" << endl;
  }

  // imprimos el vector de posibles movimientos
  // cout << "Posibles movimientos: ";
  for (const auto &movimiento : posiblesMovimientos)
  {
    // cout << traducirAccion(movimiento) << " ";
  }
  // cout << endl;

  SituarSensorEnMapaR(mapaResultado, mapaCotas, sensores);

  // pintamos el mapa de la casilla actual y la de los adyacentes
  // cout << "Mapa actual:" << endl;
  for (int i = 0; i < mapaResultado.size(); ++i)
  {
    for (int j = 0; j < mapaResultado[i].size(); ++j)
    {
      if (i == sensores.posF && j == sensores.posC)
      {
        // cout << "A "; // Representa la posición actual del agente
      }
      else
      {
        // cout << mapaResultado[i][j] << " ";
      }
    }
    // cout << endl;
  }

  // pinta el valor de sensor superficie y la de los adyacentes
  // cout << "Terreno actual: " << DescripcionTerreno(sensores.superficie[0]) << endl;
  // cout << "Terreno delante: " << DescripcionTerreno(sensores.superficie[2]) << endl;
  // cout << "Terreno derecha: " << DescripcionTerreno(sensores.superficie[3]) << endl;
  // cout << "Terreno izquierda: " << DescripcionTerreno(sensores.superficie[1]) << endl;
  // El comportamiento de seguir un camino hasta encontrar un puesto base.
  // Actualización de variables de estado
  // SituarSensorEnMapaR(mapaResultado, mapaCotas, sensores);
  if (sensores.superficie[0] == 'D')
    tiene_zapatillas = true;

  // pintamos las dimensiones del mapa usando mapaResultado
  // cout << "Dimensiones del mapa: " << mapaResultado.size() << "x" << mapaResultado[0].size() << endl;

  if (CasillVisitadamasde10veces(fila, col))
  {

    Action ac = PriorizarCaminosDeInterrogacion(sensores, fila, col);
    if (ac != IDLE)
    {
      // cout << "Entra en el if de la casilla visitada más de 10 veces" << endl;
      return ac;
    }
    else
    {
      // cout << "No hay caminos de interrogación" << endl;
    }

    bool evitarColision = evitarColisionAuxiliar(sensores, accion, giro45Izq);
    // cout << "Entra en el if de la casilla visitada más de 10 veces" << endl;
    if (evitarColision)
    {
      return accion;
    }
    else
    {
      // Priorizamos las no visitadas
      if (escasillavalida_lvl1(sensores, 3))
      {
        // cout << "Entra en el if de la casilla 3" << endl;
        return TURN_SR;
      }
      else if (escasillavalida_lvl1(sensores, 2))
      {
        // cout << "Entra en el if de la casilla 2" << endl;
        return WALK;
      }
      else if (escasillavalida_lvl1(sensores, 1))
      {
        // cout << "Entra en el if de la casilla 1" << endl;
        giro45Izq = 1;
        return TURN_L;
      }
      else
      {
        // cout << "Entra en el else de la casilla visitada más de 10 veces" << endl;
        return TURN_L;
      }
    }
  }

  if (contador > 2200)
  { // este if se usa para aumentar la aleatoriedad, y de esta manera mejorar el comporamiento reactivo del código y conseguir la meta.
    if (sensores.superficie[3] == 'X' || sensores.superficie[3] == 'C')
    {
      return TURN_SR;
    }
    else if (sensores.superficie[2] == 'X' || sensores.superficie[2] == 'C')
    {
      return WALK;
    }
    else
      return TURN_L;
  }

  // Definición del comportamiento del rescatador
  if (giro45Izq != 0)
  { // Estoy haciendo un TURN_SL
    accion = TURN_SR;
    giro45Izq--;
  }
  else if (!posiblesMovimientos.empty())
  {
    // cout << "Entra en el if de posibles movimientos" << endl;
    return posiblesMovimientos[0]; // Devolver el primer movimiento posible
  }
  else
  {

    char i = ViablePorAltura(sensores.superficie[1], sensores.cota[1] - sensores.cota[0], tiene_zapatillas);
    char c = ViablePorAltura(sensores.superficie[2], sensores.cota[2] - sensores.cota[0], tiene_zapatillas);
    char d = ViablePorAltura(sensores.superficie[3], sensores.cota[3] - sensores.cota[0], tiene_zapatillas);

    int pos = VeoCasillaInteresanteNivel1(i, c, d, tiene_zapatillas);

    switch (pos)
    {

    case 2: // Vamos a avanzar hacia delante+
      // cout << "Entra en case 2" << endl;
      if (sensores.agentes[2] == 'a' || sensores.agentes[6] == 'a' || sensores.agentes[12] == 'a')
      {

        // cout << "Evito posible colisión con Auxiliar (cerca o delante+1)" << endl;
        accion = TURN_SR;
        giro45Izq = 3;
      }
      else
        return WALK;
      break;
    case 1:
      // cout << "Entra en case 1" << endl;
      giro45Izq = 1;
      accion = TURN_L;
      break;
    case 3:
      // cout << "Entra en case 3" << endl;
      accion = TURN_SR;
      break;
    case 0:
      if (escasillavalida_lvl1(sensores, 2))
      {
        // cout << "Puede avanzar hacia adelante, elige una acción aleatoria" << endl;
        int opcion = rand() % 3; // Generar un número aleatorio entre 0 y 2
        if (opcion == 0)
        {
          return WALK; // Avanzar
        }
        else if (opcion == 1)
        {
          return TURN_L; // Girar a la izquierda
        }
        else
        {
          return TURN_SR; // Girar a la derecha
        }
      }
      break;
    }
  }

  if (last_action == accion)
  {
    acciones_Seguidas++; // aumentamos el contador de acciones seguidas, ya que hay ciertos mapas que se quedan atascados en un bucle y no avanzan, como es el caso del ajedrez.
  }
  else if (acciones_Seguidas > 0)
  {
    acciones_Seguidas--;
  }

  // cout << "#################RESCATADOR-FIN################# con terreno" << sensores.superficie[0] << endl;

  // Devolver la siguiente acción a hacer
  last_action = accion;
  return accion;
}

// ════════════════════════════════════════════════════════════════════════════

// ════════════════════════════════════════════════════════════════════════════
//                              FUNCIONES NIVEL E
// ════════════════════════════════════════════════════════════════════════════

void AnularMatrizR(vector<vector<unsigned char>> &m)
{
  for (int i = 0; i < m.size(); i++)
  {
    for (int j = 0; j < m[i].size(); j++)
    {
      m[i][j] = 0;
    }
  }
}

void ComportamientoRescatador::VisualizaPlan(const EstadoR &st, const list<Action> &plan)
{
  AnularMatrizR(mapaConPlan);
  EstadoR cst = st;
  auto it = plan.begin();
  while (it != plan.end())
  {
    switch (*it)
    {
    case RUN:
      for (int i = 0; i < 2; ++i)
      {
        // cout << "Entramos en el visualiza plan RUN" << endl;
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
        mapaConPlan[cst.f][cst.c] = 3; 
      }
      break;
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
      mapaConPlan[cst.f][cst.c] = 1; // Mark as WALK
      break;
    case TURN_SR:
      cst.brujula = (cst.brujula + 1) % 8;
      break;
    case TURN_L:
      cst.brujula = (cst.brujula + 6) % 8;
      break;
    }
    it++;
  }
}

void ComportamientoRescatador::PintaPlan(const list<Action> &plan, bool zap)
{
  auto it = plan.begin();
  while (it != plan.end())
  {
    if (*it == WALK)
    {
      cout << "W ";
    }
    else if (*it == RUN)
    {
      cout << "R ";
    }
    else if (*it == TURN_SR)
    {
      cout << "r ";
    }
    else if (*it == TURN_L)
    {
      cout << "L ";
    }
    else if (*it == CALL_ON)
    {
      cout << "C ";
    }
    else if (*it == CALL_OFF)
    {
      cout << "c ";
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
  cout << "( longitud " << plan.size();
  if (zap)
    cout << "[Z]";
  cout << ")\n";
}

bool ComportamientoRescatador::Find(const NodoR &st, const list<NodoR> &lista)
{
  auto it = lista.begin();
  while (it != lista.end() and !((*it) == st))
  {
    it++;
  }
  return (it != lista.end());
}

EstadoR ComportamientoRescatador::NextCasillaRescatador(const EstadoR &st)
{
  EstadoR siguiente = st;
  switch (st.brujula)
  {
  case norte:
    siguiente.f = st.f - 1;
    break;
  case noreste:
    siguiente.f = st.f - 1;
    siguiente.c = st.c + 1;
    break;
  case este:
    siguiente.c = st.c + 1;
    break;
  case sureste:
    siguiente.f = st.f + 1;
    siguiente.c = st.c + 1;
    break;
  case sur:
    siguiente.f = st.f + 1;
    break;
  case suroeste:
    siguiente.f = st.f + 1;
    siguiente.c = st.c - 1;
    break;
  case oeste:
    siguiente.c = st.c - 1;
    break;
  case noroeste:
    siguiente.f = st.f - 1;
    siguiente.c = st.c - 1;
    break;
  }
  if (siguiente.f < 0 || siguiente.c < 0 || siguiente.f >= mapaResultado.size() || siguiente.c >= mapaResultado.size())
    siguiente = st;
  return siguiente;
}

bool ComportamientoRescatador::CasillaAccesibleRescatador(const EstadoR &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura)
{
  EstadoR next = NextCasillaRescatador(st);
  // cout << "La casilla accesible en casillaAccesibleRescatador es: " << next.f << " " << next.c << endl;
  bool check1 = false, check2 = false, check3 = false;
  check1 = terreno[next.f][next.c] != 'P' and terreno[next.f][next.c] != 'M';            // Nos aseguramos que no es un precipicio ni una montaña
  check2 = terreno[next.f][next.c] != 'B';                                               // Sabemos que el rescatador no puede pasar por casillas de tipo bosque
  check3 = abs(altura[next.f][next.c] - altura[st.f][st.c]) <= (!st.zapatillas ? 1 : 2); // Comprobamos que la diferencia de altura es menor o igual a 1 si no tiene zapatillas, o menor o igual a 2 si tiene zapatillas
  bool check4 = false;                                                                   // Comprobamos que las casillas esten dentro del mapa
  if (next.f >= 0 && next.f < terreno.size() && next.c >= 0 && next.c < terreno[0].size())
  {
    check4 = true;
  }
  if (terreno[next.f][next.c] == '?')
  {
    check3 = true;
  }
  if (next.f == posicionAuxiliar.first && next.c == posicionAuxiliar.second)
  {
    // cout << "No se puede pasar por la casilla del auxiliar" << endl;
    // cout << "F " << next.f << " C " << next.c << endl;
    return false; // No se puede pasar por la casilla del auxiliar
  }

  // cout << "check1: " << check1 << " check2: " << check2 << " check3: " << check3 << endl;
  return check1 and check2 and check3 and check4; // Comprobamos que la casilla es accesible
}

EstadoR ComportamientoRescatador::applyR(Action accion, const EstadoR &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura)
{
  EstadoR next = st;
  switch (accion)
  {
  case WALK:
    if (CasillaAccesibleRescatador(st, terreno, altura))
    {
      next = NextCasillaRescatador(st);
    }
    break;
  case TURN_SR:
    next.brujula = (next.brujula + 1) % 8;
    break;
  case TURN_L:
    next.brujula = (next.brujula + 6) % 8;
    break;
  case RUN:
    next = NextCasillaRescatador(st);
    bool check1 = false, check2 = false, check3 = false, check4 = false, check5 = false;
    check1 = terreno[next.f][next.c] != 'P' and terreno[next.f][next.c] != 'M'; // Nos aseguramos que no es un precipicio ni una montaña
    check2 = terreno[next.f][next.c] != 'B';                                    // Sabemos que el rescatador no puede pasar por casillas de tipo bosque
    if (next.f == posicionAuxiliar.first && next.c == posicionAuxiliar.second)
    {
      // cout << "No se puede pasar por la casilla del auxiliar" << endl;
      // cout << "F " << next.f << " C " << next.c << endl;
      return st; // No se puede pasar por la casilla del auxiliar
    }
    next = NextCasillaRescatador(next);
    check3 = abs(altura[next.f][next.c] - altura[st.f][st.c]) <= (!st.zapatillas ? 1 : 2); // Comprobamos que la diferencia de altura es menor o igual a 1 si no tiene zapatillas, o menor o igual a 2 si tiene zapatillas
    check4 = terreno[next.f][next.c] != 'P' and terreno[next.f][next.c] != 'M';            // Nos aseguramos que no es un precipicio ni una montaña
    check5 = terreno[next.f][next.c] != 'B';                                               // Sabemos que el rescatador no puede pasar por casillas de tipo bosque
    if (next.f == posicionAuxiliar.first && next.c == posicionAuxiliar.second)
    {
      // cout << "No se puede pasar por la casilla del auxiliar" << endl;
      // cout << "F " << next.f << " C " << next.c << endl;
      return st; // No se puede pasar por la casilla del auxiliar
    }

    if (check1 and check2 and check3 and check4 and check5)
    {
      // cout << "La casilla es accesible DENTRO DE RESCATADOR" << endl;
      return next;
    }
    else
    {
      return st; // Si no es accesible, volvemos al estado original
    }
    break;
  }
  // vemos si estamos en una casilla que da las zapatillas
  if (terreno[next.f][next.c] == 'D')
  {
    next.zapatillas = true;
  }
  return next;
}

list<Action> ComportamientoRescatador::AnchuraRescatador(const EstadoR &inicio, const EstadoR &final, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura)
{
  NodoR current_node;
  list<NodoR> frontier;
  list<Action> path;
  set<NodoR> explored;

  current_node.estado = inicio; // Asigno el estado inicial al nodo actual.
  frontier.push_back(current_node);
  bool SolutionFound = (current_node.estado.f == final.f && current_node.estado.c == final.c);

  while (!SolutionFound and !frontier.empty())
  {
    frontier.pop_front();
    explored.insert(current_node);

    // Compruebo si estoy en una casilla que da las zapatillas
    if (terreno[current_node.estado.f][current_node.estado.c] == 'i')
    {
      current_node.estado.zapatillas = true;
    }

    // Genero el hijo resultante de aplicar la acción WALK
    NodoR child_WALK = current_node;
    child_WALK.estado = applyR(WALK, current_node.estado, terreno, altura);
    if (child_WALK.estado.f == final.f and child_WALK.estado.c == final.c)
    {
      // El hijo generado es solución
      child_WALK.secuencia.push_back(WALK);
      current_node = child_WALK;
      SolutionFound = true;
    }
    else if (explored.find(child_WALK) == explored.end())
    {
      // Se mete en la lista frontier después de añadir a secuencia la acción
      child_WALK.secuencia.push_back(WALK);
      frontier.push_back(child_WALK);
    }

    // Genero el hijo resultante de aplicar la acción TURN_SR
    if (!SolutionFound)
    {
      NodoR child_TURN_SR = current_node;
      child_TURN_SR.estado = applyR(TURN_SR, current_node.estado, terreno, altura);
      if (explored.find(child_TURN_SR) == explored.end())
      {
        child_TURN_SR.secuencia.push_back(TURN_SR);
        frontier.push_back(child_TURN_SR);
      }
    }

    // Genero el hijo resultante de aplicar la acción TURN_L
    if (!SolutionFound)
    {
      NodoR child_TURN_L = current_node;
      child_TURN_L.estado = applyR(TURN_L, current_node.estado, terreno, altura);
      if (explored.find(child_TURN_L) == explored.end())
      {
        child_TURN_L.secuencia.push_back(TURN_L);
        frontier.push_back(child_TURN_L);
      }
    }

    // Paso a evaluar el siguiente nodo en la lista "frontier"
    if (!SolutionFound and !frontier.empty())
    {
      current_node = frontier.front();
      while (explored.find(current_node) != explored.end() and !frontier.empty())
      {
        frontier.pop_front();
        current_node = frontier.front();
      }
    }
  }

  // Devuelvo el camino si se ha encontrado solución
  if (SolutionFound)
    path = current_node.secuencia;

  return path;
}

// ════════════════════════════════════════════════════════════════════════════

// ════════════════════════════════════════════════════════════════════════════
//                              NIVEL E
// ════════════════════════════════════════════════════════════════════════════

Action ComportamientoRescatador::ComportamientoRescatadorNivel_E(Sensores sensores)
{
  Action accion = IDLE;
  if (!hayPlan)
  {
    // Invocar al método de búsqueda
    EstadoR inicio, fin;
    inicio.f = sensores.posF;
    inicio.c = sensores.posC;
    inicio.brujula = sensores.rumbo;
    // cout << "El valor de tiene_zapatillas antes de asignar es: " << tiene_zapatillas << endl;
    inicio.zapatillas = tiene_zapatillas;
    // cout << "En el nivel E el valor de tiene_zapatillas es: " << tiene_zapatillas << endl;
    fin.f = sensores.destinoF;
    fin.c = sensores.destinoC;
    plan = AnchuraRescatador(inicio, fin, mapaResultado, mapaCotas);
    VisualizaPlan(inicio, plan);
    hayPlan = plan.size() != 0;
  }
  if (hayPlan and plan.size() > 0)
  {
    accion = plan.front();
    plan.pop_front();
  }
  if (plan.size() == 0)
  {
    hayPlan = false;
  }
  return accion;
}

// ════════════════════════════════════════════════════════════════════════════

// ════════════════════════════════════════════════════════════════════════════
//                              FUNCIONES NIVEL 2
// ════════════════════════════════════════════════════════════════════════════

int ComportamientoRescatador::CosteBaseTerreno(unsigned char casilla, Action accion)
{
  switch (accion)
  {
  case WALK:
    switch (casilla)
    {
    case 'A':
      return 100;
    case 'T':
      return 20;
    case 'S':
      return 2;
    default:
      return 1;
    }
  case RUN:
    switch (casilla)
    {
    case 'A':
      return 150;
    case 'T':
      return 35;
    case 'S':
      return 3;
    default:
      return 1;
    }
  case TURN_L:
    switch (casilla)
    {
    case 'A':
      return 30;
    case 'T':
      return 5;
    case 'S':
      return 1;
    default:
      return 1;
    }
  case TURN_SR:
    switch (casilla)
    {
    case 'A':
      return 16;
    case 'T':
      return 3;
    case 'S':
      return 1;
    default:
      return 1;
    }
  default:
    return 1;
  }
}

int ComportamientoRescatador::CosteCambioAltura(int altura_origen, int altura_destino, unsigned char casilla, Action accion)
{
  if (accion != WALK && accion != RUN)
    return 0; // Solo aplica en movimientos, no en giros

  int diferencia = altura_destino - altura_origen;
  if (diferencia == 0)
    return 0;

  switch (accion)
  {
  case WALK:
    switch (casilla)
    {
    case 'A':
      return (diferencia > 0) ? 10 : -10;
    case 'T':
      return (diferencia > 0) ? 5 : -5;
    case 'S':
      return (diferencia > 0) ? 1 : -1;
    default:
      return 0; // El resto de terrenos no aplican modificación
    }
  case RUN:
    switch (casilla)
    {
    case 'A':
      return (diferencia > 0) ? 15 : -15;
    case 'T':
      return (diferencia > 0) ? 5 : -5;
    case 'S':
      return (diferencia > 0) ? 2 : -2;
    default:
      return 0; // El resto de terrenos no aplican modificación
    }
  default:
    return 0;
  }
}

list<Action> ComportamientoRescatador::DijkstraRescatador(const EstadoR &origen, const EstadoR &destino, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura)
{
  NodoR current_nodo;
  priority_queue<NodoR, vector<NodoR>, greater<NodoR>> frontier;
  set<NodoR> explored;
  list<Action> path;

  current_nodo.estado = origen;
  current_nodo.coste = 0;
  current_nodo.secuencia.clear();
  frontier.push(current_nodo);

  // Inicializamos matriz de coste mínimo: [filas][columnas][8 orientaciones][zapatillas, las tiene o no]
  int filas = terreno.size();
  int columnas = terreno[0].size();
  vector<vector<vector<vector<int>>>> costeMinimo(
      filas, vector<vector<vector<int>>>(
                 columnas, vector<vector<int>>(
                               8, vector<int>(2, INT_MAX)))); 

  costeMinimo[origen.f][origen.c][origen.brujula][origen.zapatillas ? 1 : 0] = 0;

  while (!frontier.empty())
  {
    current_nodo = frontier.top();
    frontier.pop();

    // Si hemos llegado al destino (ignora orientación y zapatillas)
    if (current_nodo.estado.f == destino.f && current_nodo.estado.c == destino.c)
    {
      return current_nodo.secuencia; // Devolvemos la secuencia de acciones si es la solución
    }

    // // cout << "Visitando (" << current_nodo.estado.f << "," << current_nodo.estado.c << ") orient: " << current_nodo.estado.brujula << " zap: " << current_nodo.estado.zapatillas << " coste: " << current_nodo.coste << endl;

    explored.insert(current_nodo);

    // Recoger zapatillas si estamos sobre 'D'
    if (terreno[current_nodo.estado.f][current_nodo.estado.c] == 'D')
    {
      current_nodo.estado.zapatillas = true;
    }

    // Acciones posibles
    vector<Action> acciones = {WALK, RUN, TURN_SR, TURN_L};

    for (Action accion : acciones)
    { // Hemos decidido usar un bucle para evitar repetir código
      NodoR hijo = current_nodo;
      hijo.estado = applyR(accion, current_nodo.estado, terreno, altura);
      int costeTerreno = CosteBaseTerreno(terreno[current_nodo.estado.f][current_nodo.estado.c], accion);
      int costeAltura = (accion == WALK || accion == RUN) ? CosteCambioAltura(
                                                                altura[current_nodo.estado.f][current_nodo.estado.c],
                                                                altura[hijo.estado.f][hijo.estado.c],
                                                                terreno[current_nodo.estado.f][current_nodo.estado.c],
                                                                accion)
                                                          : 0;

      hijo.coste = current_nodo.coste + costeTerreno + costeAltura;
      hijo.secuencia = current_nodo.secuencia;
      hijo.secuencia.push_back(accion);

      int f = hijo.estado.f;
      int c = hijo.estado.c;
      int ori = hijo.estado.brujula;
      int zap = hijo.estado.zapatillas ? 1 : 0;

      if (f >= 0 && f < filas && c >= 0 && c < columnas)
      {
        if (hijo.coste < costeMinimo[f][c][ori][zap])
        {
          costeMinimo[f][c][ori][zap] = hijo.coste;
          frontier.push(hijo);
        }
      }
    }
  }

  // cout << "No se encontró un camino al destino." << endl;
  return path;
}

// ════════════════════════════════════════════════════════════════════════════

// ════════════════════════════════════════════════════════════════════════════
//                              NIVEL 2
// ════════════════════════════════════════════════════════════════════════════

Action ComportamientoRescatador::ComportamientoRescatadorNivel_2(Sensores sensores)
{
  Action accion = IDLE;
  if (!hayPlan)
  {
    // Invocar al método de búsqueda
    EstadoR inicio, fin;
    inicio.f = sensores.posF;
    inicio.c = sensores.posC;
    inicio.brujula = sensores.rumbo;
    // cout << "El valor de tiene_zapatillas antes de asignar es: " << tiene_zapatillas << endl;
    inicio.zapatillas = tiene_zapatillas;
    // cout << "En el nivel 2 el valor de tiene_zapatillas es: " << tiene_zapatillas << endl;
    fin.f = sensores.destinoF;
    fin.c = sensores.destinoC;
    plan = DijkstraRescatador(inicio, fin, mapaResultado, mapaCotas);
    // pintamos el plan
    // cout << "Plan encontrado: ";
    PintaPlan(plan, tiene_zapatillas);
    VisualizaPlan(inicio, plan);
    hayPlan = plan.size() != 0;
  }
  if (hayPlan and plan.size() > 0)
  {
    accion = plan.front();
    plan.pop_front();
  }
  if (plan.size() == 0)
  {
    hayPlan = false;
  }

  return accion;
}

// ════════════════════════════════════════════════════════════════════════════

Action ComportamientoRescatador::ComportamientoRescatadorNivel_3(Sensores sensores)
{
  return IDLE;
}

// ════════════════════════════════════════════════════════════════════════════
//                              FUNCIONES NIVEL 4
// ════════════════════════════════════════════════════════════════════════════

bool ComportamientoRescatador::casillValidad_lvl4(Sensores sensores, int pos)
{
  if (sensores.superficie[pos] != 'M' && sensores.superficie[pos] != 'P' && sensores.superficie[pos] != 'B' && ViablePorAltura(sensores.superficie[pos], sensores.cota[pos] - sensores.cota[0], tiene_zapatillas) != 'P')
  {
    return true;
  }
  else
  {
    return false;
  }
}

int ComportamientoRescatador::hayVandaloExcursionista(Sensores sensores, int pos)
{
  return 1;
}
bool enemigoCerca(const vector<unsigned char> &agentes)
{
  // Chequeamos las posiciones alrededor del agente
  // Puedes ajustar estos índices según cómo represente tu entorno
  vector<int> indicesPeligrosos = {1, 2, 3, 5, 6, 7, 11, 12, 13};

  for (int i : indicesPeligrosos)
  {
    if (agentes[i] == 'e' || agentes[i] == 'v')
    {
      // cout << "Enemigo detectado en sensores.agentes[" << i << "]: " << agentes[i] << endl;
      return true;
    }
  }
  return false;
}

bool ComportamientoRescatador::decirleAlAuxiliar(Sensores sensores)
{
  for (int i = 0; i < 16; i++)
  {
    if (sensores.agentes[i] == 'a')
    {
      return true;
    }
  }
  return false;
}

bool casillaTransitable(unsigned char terreno)
{
  return terreno == 'C' || terreno == 'S' || terreno == 'T' || terreno == 'A';
}

int contarInterrogantesAlrededor(const vector<vector<unsigned char>> &mapa, int f, int c)
{
  int total = 0;
  for (int df = -1; df <= 1; ++df)
  {
    for (int dc = -1; dc <= 1; ++dc)
    {
      if (df == 0 && dc == 0)
        continue; // Saltar la casilla central
      int nf = f + df;
      int nc = c + dc;
      if (nf >= 0 && nf < mapa.size() && nc >= 0 && nc < mapa[0].size())
      {
        if (mapa[nf][nc] == '?')
          total++;
      }
    }
  }
  return total;
}

// función para ver las casillas con mas interrogaciones alrededor
priority_queue<CasillaDescubrimiento> ComportamientoRescatador::cola_con_casillas_con_interrogacion(Sensores sensores)
{
  priority_queue<CasillaDescubrimiento> explorables;

  for (int f = 1; f < mapaResultado.size() - 1; ++f)
  {
    for (int c = 1; c < mapaResultado[0].size() - 1; ++c)
    {
      if (!visitadosCasillasInterrogacion[f][c])
      {
        if (!casillaTransitable(mapaResultado[f][c]))
          continue;

        int numInterrogantes = contarInterrogantesAlrededor(mapaResultado, f, c);
        if (numInterrogantes > 0)
        {
          explorables.push({f, c, numInterrogantes});
          visitadosCasillasInterrogacion[f][c] = true; // Marcar como visitada
        }
      }
    }
  }
  return explorables;
}

double porcentajeMapaDescubierto(const vector<vector<unsigned char>> &mapa)
{
  int totalCasillas = mapa.size() * mapa[0].size();
  // cout << "Total de casillas en el mapa: " << totalCasillas << endl;
  int casillasDescubiertas = 0;
  for (const auto &fila : mapa)
  {
    for (unsigned char casilla : fila)
    {
      if (casilla != '?')
      {
        casillasDescubiertas++;
      }
    }
  }
  // cout << "Total de casillas descubiertas: " << casillasDescubiertas << endl;
  // cout << "Porcentaje de mapa descubierto: " << static_cast<double>(casillasDescubiertas) / totalCasillas * 100.0 << "%" << endl;
  return static_cast<double>((casillasDescubiertas) / totalCasillas) * 100.0;
}

void AsignarAgrupacionesdeAguacomoMuro(Sensores sensores, vector<vector<unsigned char>> &mapaResultado, vector<vector<unsigned char>> &mapaCotas)
{
  for (int i = 1; i < mapaResultado.size() - 1; i++)
  {
    for (int j = 1; j < mapaResultado[0].size() - 1; j++)
    {
      if (mapaResultado[i][j] == 'A')
      {
        int aguaAlrededor = 0;

        // Contar casillas de agua alrededor
        for (int di = -1; di <= 1; di++)
        {
          for (int dj = -1; dj <= 1; dj++)
          {
            if (di == 0 && dj == 0)
              continue; // Ignorar la casilla actual
            int ni = i + di;
            int nj = j + dj;
            if (ni >= 0 && ni < mapaResultado.size() && nj >= 0 && nj < mapaResultado[0].size() && mapaResultado[ni][nj] == 'A')
            {
              aguaAlrededor++;
            }
          }
        }

        if (i >= 0 && i < mapaResultado.size() && j >= 0 && j < mapaResultado[0].size())
        {
          mapaResultado[i][j] = 'M';
          mapaCotas[i][j] = 0;
        }
        mapaResultado[i][j] = 'M';
        mapaCotas[i][j] = 0;
      }
    }
  }
}

list<Action> ComportamientoRescatador::AEstrellaRescatador(
    const EstadoR &origen,
    const EstadoR &destino,
    const vector<vector<unsigned char>> &terreno,
    const vector<vector<unsigned char>> &altura)
{

  NodoR current_nodo;
  priority_queue<NodoR, vector<NodoR>, greater<NodoR>> frontier;
  set<NodoR> explored;
  list<Action> path;

  current_nodo.estado = origen;
  current_nodo.coste = 0;
  current_nodo.secuencia.clear();
  frontier.push(current_nodo);

  int filas = terreno.size();
  int columnas = terreno[0].size();
  vector<vector<vector<vector<int>>>> costeMinimo(
      filas, vector<vector<vector<int>>>(
                 columnas, vector<vector<int>>(
                               8, vector<int>(2, INT_MAX))));

  costeMinimo[origen.f][origen.c][origen.brujula][origen.zapatillas ? 1 : 0] = 0;

  while (!frontier.empty())
  {
    current_nodo = frontier.top();
    frontier.pop();

    // Si hemos llegado al destino
    if (current_nodo.estado.f == destino.f && current_nodo.estado.c == destino.c)
    {
      return current_nodo.secuencia;
    }

    explored.insert(current_nodo);

    // Recoger zapatillas si estamos sobre 'D'
    if (terreno[current_nodo.estado.f][current_nodo.estado.c] == 'D')
    {
      current_nodo.estado.zapatillas = true;
    }

    vector<Action> acciones = {WALK, RUN, TURN_SR, TURN_L};

    for (Action accion : acciones)
    {
      NodoR hijo = current_nodo;
      hijo.estado = applyR(accion, current_nodo.estado, terreno, altura);

      int costeTerreno = CosteBaseTerreno(terreno[current_nodo.estado.f][current_nodo.estado.c], accion);
      int costeAltura = (accion == WALK || accion == RUN) ? CosteCambioAltura(
                                                                altura[current_nodo.estado.f][current_nodo.estado.c],
                                                                altura[hijo.estado.f][hijo.estado.c],
                                                                terreno[current_nodo.estado.f][current_nodo.estado.c],
                                                                accion)
                                                          : 0;

      int nuevoCoste = current_nodo.coste + costeTerreno + costeAltura;

      hijo.coste = nuevoCoste;
      hijo.secuencia = current_nodo.secuencia;
      hijo.secuencia.push_back(accion);

      int f = hijo.estado.f;
      int c = hijo.estado.c;
      int ori = hijo.estado.brujula;
      int zap = hijo.estado.zapatillas ? 1 : 0;

      if (f >= 0 && f < filas && c >= 0 && c < columnas)
      {
        if (nuevoCoste < costeMinimo[f][c][ori][zap])
        {
          costeMinimo[f][c][ori][zap] = nuevoCoste;

          // AÑADIMOS LA HEURÍSTICA (distancia de Manhattan)
          int heuristica = abs(destino.f - f) + abs(destino.c - c);
          int prioridad = nuevoCoste + heuristica;

          // Guardamos el valor de prioridad como coste en el nodo para la priority_queue
          NodoR nodoPrioridad = hijo;
          nodoPrioridad.coste = prioridad;

          frontier.push(nodoPrioridad);
        }
      }
    }
  }

  // cout << "No se encontró un camino al destino con A*." << endl;
  return path;
}

// ════════════════════════════════════════════════════════════════════════════

// ════════════════════════════════════════════════════════════════════════════
//                              NIVEL 4
// ════════════════════════════════════════════════════════════════════════════

Action ComportamientoRescatador::ComportamientoRescatadorNivel_4(Sensores sensores)
{
  // cout << "#####@@@@@#####RESCATADOR-INICIO@@@@@@@@@@@@@@@@@@@" << endl;
  // cout << "El tamaño del mapa es: " << mapaResultado.size() << " " << mapaResultado[0].size() << endl;

  // SITUAR SENSOR EN MAPA ------------
  SituarSensorEnMapaR(mapaResultado, mapaCotas, sensores);
  Action accion = IDLE;
  int cuandoRecargar = 1000; // energia a la que recargamos

  if (mapaResultado.size() == 100)
  {
    cuandoRecargar = 1500;
  }
  // SITUAR SENSOR EN MAPA ------------

  contador++; // para depurar

  // cout << "El contador es: " << contador << endl;
  // cout << "Sensores. tiempo: " << sensores.tiempo << endl;

  // imprimirmos posicion del auxiliar
  // cout << "Posicion del auxiliar: " << posicionAuxiliar.first << " " << posicionAuxiliar.second << endl;

  // OPERACIONES INICIALES -----------
  if (giro45Izq > 0)
  {
    giro45Izq--;
    return TURN_SR;
  }

  if (sensores.superficie[0] == 'D')
  {
    tiene_zapatillas = true;
  }

  // POSICION ---------------------

  pair<int, int> posicionRescatador = make_pair(sensores.posF, sensores.posC);
  if (posicionRescatador == lastPosition)
  {
    contadorMismaPosicion++;
  }
  else
  {
    contadorMismaPosicion = 0;
  }
  lastPosition = posicionRescatador;

  // POSICION ---------------------

  // OPERACIONES INICIALES -----------

  // RECARGA DE ENERGIA--------------
  // cout << "El sensores.energia es: " << sensores.energia << endl;
  if (sensores.energia >= 2999 && recargaEnergia)
  {
    recargaEnergia = false; // si la energia es 3000, quiere decir que ya hemos recargado
  }

  if (sensores.energia < cuandoRecargar)
  {
    // cout << "Entra en el if de la energia < 1000" << endl;
    recargaEnergia = true;

    // Recorremos mapaResultado para ver si hay una casilla de recarga
    for (int i = 0; i < mapaResultado.size(); i++)
    {
      for (int j = 0; j < mapaResultado[0].size(); j++)
      {
        if (mapaResultado[i][j] == 'X')
        {
          listaPuestosRecarga.insert(make_pair(i, j));
        }
      }
    }

    // La imprimimos para ver si hay alguna
    // cout << "Lista de puestos de recarga: " << endl;
    for (const auto &p : listaPuestosRecarga)
    {
      // cout << "Puesto de recarga: " << p.first << " " << p.second << endl;
    }
  }

  if (sensores.superficie[0] == 'X')
  {
    listaPuestosRecarga.insert(make_pair(sensores.posF, sensores.posC));
    estoyRecargando = true;
  }
  else
    estoyRecargando = false;

  if ((!estoyRecargando && recargaEnergia))
  { // si no estoy recargando y la energia es menor de 500, debo de trazar un plan hacia una casilla de descarga
    if (listaPuestosRecarga.size() > 0)
    {
      // cout << "Entra en el if de listaPuestosRecarga.size() > 0" << endl;

      // tenemos que hacer un plan hacia la casilla de recarga
      EstadoR inicio, fin;
      inicio.f = sensores.posF;
      inicio.c = sensores.posC;
      inicio.brujula = sensores.rumbo;
      inicio.zapatillas = tiene_zapatillas;
      // calculamos el más cercano
      pair<int, int> masCercano;
      int minDistancia = INT_MAX;

      for (const auto &p : listaPuestosRecarga)
      {
        int distancia = abs(p.first - sensores.posF) + abs(p.second - sensores.posC);
        if (distancia < minDistancia)
        {
          minDistancia = distancia;
          masCercano = p;
        }
      } // distancia de manhattan

      fin.f = masCercano.first;
      fin.c = masCercano.second;

      plan = AEstrellaRescatador(inicio, fin, mapaResultado, mapaCotas);
      VisualizaPlan(inicio, plan);
      hayPlan = plan.size() != 0;
    }
    else
    {
      // cout << "Entra en el else de listaPuestosRecarga.size() > 0" << endl;
      // no hay puestos de recarga, por lo que no podemos hacer nada
    }
  }

  if (recargaEnergia && estoyRecargando)
  {
    return IDLE;
  }

  // RECARGA DE ENERGIA--------------

  // BUSCAMOS DESTINO ----------------

  if (!hayPlan)
  {
    EstadoR inicio, fin;
    inicio.f = sensores.posF;
    inicio.c = sensores.posC;
    inicio.brujula = sensores.rumbo;
    inicio.zapatillas = tiene_zapatillas;
    fin.f = sensores.destinoF;
    fin.c = sensores.destinoC;
    plan = AEstrellaRescatador(inicio, fin, mapaResultado, mapaCotas);
    VisualizaPlan(inicio, plan);
    hayPlan = plan.size() != 0;
    // cout << "Plan encontrado de tamaño: " << plan.size() << endl;
    PintaPlan(plan, tiene_zapatillas);

    if (plan.size() == 0 && mapaResultado.size() <= 30)
    {
      // si no hay plan y el mapa es pequeño, tenemos que buscar la casilla con '?' más cercana
      // cout << "Entra en el if de plan.size() == 0 && mapaResultado.size() <= 30" << endl;
      priority_queue<CasillaDescubrimiento> explorables = cola_con_casillas_con_interrogacion(sensores);
      if (!explorables.empty())
      {
        CasillaDescubrimiento casilla = explorables.top();
        EstadoR inicio, fin;
        inicio.f = sensores.posF;
        inicio.c = sensores.posC;
        inicio.brujula = sensores.rumbo;
        inicio.zapatillas = tiene_zapatillas;

        fin.f = casilla.f;
        fin.c = casilla.c;

        plan = AEstrellaRescatador(inicio, fin, mapaResultado, mapaCotas);
        VisualizaPlan(inicio, plan);
        hayPlan = plan.size() != 0;
      }
    }
  }

  if (hayPlan && plan.size() > 0)
  {
    // cout << "Entra en el if de hayPlan y el tamaño de plan es > 0" << endl;
    // cout << "El objetivo actual es: " << objetivoActual.first << " " << objetivoActual.second << endl;
    // cout << "La pos actual es: " << sensores.posF << " " << sensores.posC << endl;

    accion = plan.front();
    plan.pop_front();
  }

  if (plan.size() == 0)
  {
    // cout << "Entra en el if de plan.size() == 0" << endl;
    hayPlan = false;

    bool gravedad = sensores.gravedad;
    if (gravedad)
    {
      // cout << "Entra en el if de gravedad" << endl;
      // debemos de coger siempre la misión y ver si llegamos
      accion = CALL_ON;
      // cout << "la accion es call on" << endl;
    }

    if (contadorMismaPosicion > 10 && !gravedad)
    {
      // Buscar la casilla con '?' más cercana
      pair<int, int> casillaInterrogacionMasCercana;
      int distanciaMinima = INT_MAX;

      for (int i = 0; i < mapaResultado.size(); ++i)
      {
        for (int j = 0; j < mapaResultado[0].size(); ++j)
        {
          if (mapaResultado[i][j] == '?')
          {
            int distancia = abs(i - sensores.posF) + abs(j - sensores.posC);
            if (distancia < distanciaMinima)
            {
              distanciaMinima = distancia;
              casillaInterrogacionMasCercana = {i, j};
            }
          }
        }
      }

      if (distanciaMinima != INT_MAX)
      {
        // Generar un plan hacia la casilla con '?' más cercana
        EstadoR inicio, fin;
        inicio.f = sensores.posF;
        inicio.c = sensores.posC;
        inicio.brujula = sensores.rumbo;
        inicio.zapatillas = tiene_zapatillas;

        fin.f = casillaInterrogacionMasCercana.first;
        fin.c = casillaInterrogacionMasCercana.second;

        plan = AEstrellaRescatador(inicio, fin, mapaResultado, mapaCotas);
        VisualizaPlan(inicio, plan);
        hayPlan = plan.size() != 0;
      }
    }
  }

  // BUSCAMOS DESTINO ----------------

  // COMPROBACIÓN ADICIONAL Y CÁLCULO DE LA POSICIÓN DEL AUXILIAR PARA CONSIDERARLA COMO MURO---------------------------
  if (accion == RUN)
  {
    if (sensores.agentes[6] == 'a' || sensores.agentes[2] == 'a')
    {
      EstadoR actual;
      actual.f = sensores.posF;
      actual.c = sensores.posC;
      actual.brujula = sensores.rumbo;
      actual.zapatillas = tiene_zapatillas;
      actual = applyR(accion, actual, mapaResultado, mapaCotas);
      posicionAuxiliar = make_pair(actual.f, actual.c);
      // cout << "Entra en el if de la comprobacion de la accion RUN 3" << endl;
    }
    EstadoR actual, sig;
    actual.f = sensores.posF;
    actual.c = sensores.posC;
    actual.brujula = sensores.rumbo;
    actual.zapatillas = tiene_zapatillas;
    sig = applyR(accion, actual, mapaResultado, mapaCotas);
    if (actual == sig)
    {
      accion = IDLE;
    }
  }

  if (accion == WALK)
  {
    if (sensores.agentes[2] == 'a')
    {
      EstadoR actual;
      actual.f = sensores.posF;
      actual.c = sensores.posC;
      actual.brujula = sensores.rumbo;
      actual.zapatillas = tiene_zapatillas;
      actual = applyR(accion, actual, mapaResultado, mapaCotas);
      posicionAuxiliar = make_pair(actual.f, actual.c);
      // cout << "Entra en el if de la comprobacion de la accion WALK 3" << endl;
    }
    EstadoR actual, sig;
    actual.f = sensores.posF;
    actual.c = sensores.posC;
    actual.brujula = sensores.rumbo;
    actual.zapatillas = tiene_zapatillas;
    sig = applyR(accion, actual, mapaResultado, mapaCotas);
    if (actual == sig)
    {
      accion = IDLE;
    }
  }
  // COMPROBACIÓN ADICIONAL Y CÁLCULO DE LA POSICIÓN DEL AUXILIAR PARA CONSIDERARLA COMO MURO---------------------------

  // SI ANTES HEMOS HECHO CALL_ON DEVOLVEMOS CALL_OFF----------
  if (sensores.gravedad)
  {
    accion = CALL_ON;
  }
  if (sensores.gravedad && last_action == CALL_ON)
  {
    accion = IDLE;
    hayPlan = false;
    plan.clear();
  }

  // SI ANTES HEMOS HECHO CALL_ON DEVOLVEMOS CALL_OFF----------

  last_action = accion;
  // cout << "La accion es: " << traducirAccion(accion) << endl;
  // cout << "#################RESCATADOR-FIN#################" << endl;
  return accion;
}