#include "../Comportamientos_Jugador/auxiliar.hpp"
#include <iostream>
#include "motorlib/util.h"
#include <utility>
using namespace std;
#include <vector>
#include <algorithm>

/*
CABE DESTACAR QUE HAY FUNCIONES EXTRAS QUE SE IMPLEMENTARON PERO QUE AL FINAL NO SE USARON
*/

// VERSION FINAL

Action ComportamientoAuxiliar::think(Sensores sensores)
{
    Action accion = IDLE;

    switch (sensores.nivel)
    {
    case 0:
        accion = ComportamientoAuxiliarNivel_0(sensores);
        break;
    case 1:
        accion = ComportamientoAuxiliarNivel_1(sensores);
        break;
    case 2:
        accion = ComportamientoAuxiliarNivel_2(sensores);
        break;
    case 3:
        accion = ComportamientoAuxiliarNivel_3(sensores);
        // accion = ComportamientoAuxiliarNivel_E(sensores);
        break;
    case 4:
        accion = ComportamientoAuxiliarNivel_4(sensores);
        break;
    }

    return accion;
}

int ComportamientoAuxiliar::interact(Action accion, int valor)
{
    return 0;
}
// A diferencia del anterior implementado en rescatador, en este eliminado el booleano zap, ya que este no las tiene, esto se aplica a VeoCasillaInteresanteA y ViablePorAlturaA

// ════════════════════════════════════════════════════════════════════════════
//                              FUNCIONES NIVEL 0
// ════════════════════════════════════════════════════════════════════════════

// Sabemos que i es izq, que c es centro y que d es der
// Sabemos que si devuelve 2 es porque la opción más interesante es la de delante, si es 1 es la de 45º a la izq y si es 3 es la de 45º a la der
int ComportamientoAuxiliar::VeoCasillaInteresante(char i, char c, char d)
{
    if (c == 'X')
        return 2;
    else if (i == 'X')
        return 1;
    else if (d == 'X')
        return 3;
    else if (c == 'D')
        return 2;
    else if (i == 'D')
        return 1;
    else if (d == 'D')
        return 3;

    if (c == 'C')
        return 2;
    else if (i == 'C')
        return 1;
    else if (d == 'C')
        return 3;
    else
        return 0;
}
string DescripcionAccion(Action accion)
{ // Descripción de las acciones, para facilitar la depuración
    switch (accion)
    {
    case WALK:
        return "Avanzar hacia adelante.";
    case TURN_SR:
        return "Girar suavemente a la derecha.";
    case IDLE:
        return "No hacer nada.";
    default:
        return "Acción no reconocida.";
    }
}

char ComportamientoAuxiliar::ViablePorAltura(char casilla, int dif)
{
    if (abs(dif) <= 1) // el límite es 1 en todos los casos
        return casilla;
    else
        return 'P';
}
string ComportamientoAuxiliar::DescripcionTerreno(char terreno)
{
    switch (terreno)
    {
    case 'B':
        return "Bosque: Casilla no transitable, aunque podría ser transitable bajo ciertas circunstancias.";
    case 'T':
        return "Matorral: Terreno abrupto, transitable pero costoso avanzar.";
    case 'A':
        return "Agua: Obstáculo transitable para algunos agentes, pero consume mucha energía.";
    case 'P':
        return "Preci picio: Casilla intransitable, caer en ella supone el final de la simulación.";
    case 'C':
        return "Camino: Lugar fácil para caminar, conecta puestos base.";
    case 'S':
        return "Sendero: Transitable, pero consume más energía que los caminos.";
    case 'M':
        return "Obstáculo: No transitable, compuesto por antiguas construcciones o piedras grandes.";
    case 'D':
        return "Zapatillas: Mejora habilidades del agente al pasar por esta casilla.";
    case 'X':
        return "Puesto Base: Representa un objetivo o lugar importante.";
    case '?':
        return "Desconocido: Parte del mundo aún no explorada.";
    default:
        return "Terreno no reconocido.";
    }
}

/*Las dos funciones auxiliares que se encargan de contar el agua y los senderos,
se trata de un caso específico del mapa de Paldea, ya que como en el mapa de las islas hay muchas casillas de agua, hago que si hay cierta cantidad de agua,
adopte un comportamiento diferente, priorizando unas direcciones específicas para encontrar el camino más rápido hacia el objetivo, siempre realizando acciones reactivas.

*/
void ComportamientoAuxiliar::ContarAgua(Sensores sensores)
{
    for (int i = 1; i <= 3; i++)
    {
        if (sensores.superficie[i] == 'A')
        {
            vecesAgua++;
        }
    }
}

void ComportamientoAuxiliar::ContarSenderos(Sensores sensores)
{
    for (int i = 1; i <= 3; i++)
    {
        if (sensores.superficie[i] == 'S')
        {
            vecesSenderos++;
        }
    }
}

// ═════════════════════════════════════════════════════════════════════════════

// ════════════════════════════════════════════════════════════════════════════
//                              NIVEL 0
// ════════════════════════════════════════════════════════════════════════════

Action ComportamientoAuxiliar::ComportamientoAuxiliarNivel_0(Sensores sensores)
{
    Action accion;
    // Action last_action = IDLE;
    contador++;
    // cout << "############AUXILIAR-INICIO######################" << endl;
    ContarAgua(sensores);
    if (vecesAgua > 55)
    {
        hayAgua = true;
    }
    ContarSenderos(sensores);
    if (vecesSenderos > 20)
    {
        haySenderos = true;
    }
    if (haySenderos)
    {
        hayAgua = false;
    }
    // Definición del comportamiento
    if (sensores.superficie[0] == 'X')
    { // Llegué al Objetivo
        // cout << "Entra en el if de la X" << ", la accion es " << DescripcionAccion(accion) << "y quedan " << giro45Izq << " giros" << endl;
        return IDLE;
    }
    else if (giro45Izq != 0)
    { // Estoy haciendo un TURN_SL
        accion = TURN_SR;
        giro45Izq--;
    }
    else if (hayAgua)
    {
        // cout << "Entra en si hay agua" << endl;
        if (sensores.superficie[3] == 'X' || sensores.superficie[3] == 'C')
        {
            return TURN_SR;
        }
        else if (sensores.superficie[2] == 'X' || sensores.superficie[2] == 'C')
        {
            return WALK;
        }
        else
        {
            giro45Izq = 5;
            return TURN_SR; // es como girar 45 izquierda
        }
    }
    else
    {

        char i = ViablePorAltura(sensores.superficie[1], sensores.cota[1] - sensores.cota[0]);
        char c = ViablePorAltura(sensores.superficie[2], sensores.cota[2] - sensores.cota[0]);
        char d = ViablePorAltura(sensores.superficie[3], sensores.cota[3] - sensores.cota[0]);

        int pos = VeoCasillaInteresante(i, c, d);

        switch (pos)
        {
            contador_Acciones++; // contador de acciones que nos ayuda a evitar bucles, y cuand alcanzamos un número de acciones, elegimos un comportamiento aleatorio, sin ningún plan
        case 2:                  // Vamos a avanzar hacia delante
            // cout << "Entra en case 2::AUXILIAR" << endl;
            if (sensores.agentes[2] == 'r' || sensores.agentes[6] == 'r' || sensores.agentes[12] == 'r')
            {
                // cout << "Evito posible colisión con Rescatador (cerca o delante+1)" << endl;
                accion = TURN_SR; // Gira a la derecha para evitar colisión
                giro45Izq = 3;    // Marca que está girando
            }
            else
            {
                // cout << "11111111111111111111111111111" << endl;
                // Verificar opciones transitables
                bool delante = (sensores.superficie[2] == 'C' || sensores.superficie[2] == 'X');
                bool derecha = (sensores.superficie[3] == 'C' || sensores.superficie[3] == 'X');
                bool izquierda = (sensores.superficie[1] == 'C' || sensores.superficie[1] == 'X');

                // Si hay múltiples opciones, elegir una aleatoria
                if (delante && derecha && izquierda)
                {
                    // cout << "Entra en el if donde todos los caminos son transitables" << endl;
                    int opcion = rand() % 3; // Generar un número aleatorio entre 0 y 2
                    if (opcion == 0)
                    {
                        return WALK; // Avanzar
                    }
                    else if (opcion == 1)
                    {
                        accion = TURN_SR; // Girar a la izquierda
                        giro45Izq = 6;
                    }
                    else
                    {
                        accion = TURN_SR; //
                    }
                }
                else if (delante && derecha)
                {
                    // cout << "Entra en el if donde hay dos caminos transitables, delante y derecha" << endl;
                    int opcion = rand() % 2; // Generar un número aleatorio entre 0 y 1
                    if (opcion == 0)
                    {
                        return WALK; // Avanzar
                    }
                    else
                    {
                        accion = TURN_SR;
                    }
                }
                else if (delante && izquierda)
                {
                    // cout << "Entra en el if donde hay dos caminos transitables, delante y izquierda" << endl;
                    int opcion = rand() % 2; // Generar un número aleatorio entre 0 y 1
                    if (opcion == 0)
                    {
                        return WALK; // Avanzar
                    }
                    else
                    {
                        accion = TURN_SR; // Girar a la izquierda
                        giro45Izq = 6;
                    }
                }
                else
                {
                    // cout << "222222222222222222222222222222" << endl;
                    contador_Acciones++;
                    if (contador_Acciones > MAX_ACCIONES)
                    {
                        // cout << "Entra en el if donde no hay caminos transitables Y MUCHASSSSS OPCIONES" << endl;
                        // calcula un random entre 0,1,2 y si la casilla es transitable la pasa.
                        // cout << "333333333333333333333333333" << endl;
                        int opcion = rand() % 3; // Generar un número aleatorio entre 0 y 2
                        if (opcion == 0)
                        {
                            giro45Izq = 6;
                            return TURN_SR;
                        }
                        else if (opcion == 1)
                        {
                            return TURN_SR;
                        }
                        else if (opcion == 2)
                        {
                            return WALK;
                        }
                    }
                    else
                    {
                        int random = rand() % 2;
                        if (random == 0)
                        {
                            return WALK;
                        }
                        else
                        {
                            giro45Izq = 6;
                            return TURN_SR;
                        }
                    }
                }
            }
            break;
        case 1:
            // cout << "Entra en case 1::Auxiliar" << endl;
            giro45Izq = 6; // gira 5 veces 45º a la derecha = 270º = mirar hacia la izquierda
            // accion = TURN_L;
            accion = TURN_SR;
            break;
        case 3:
            // cout << "Entra en case 3::Auxiliar" << endl;
            accion = TURN_SR;
            break;
        case 0:
            // cout << "Entra en case 0::Auxiliar" << endl;
            giro45Izq = 5;
            accion = TURN_SR;
            break;
        }
    }

    // save = accion; // usamos save para guardar la última acción, podríamos usar last_action, pero en mi caso decidí incializarla de nuevo al inicio de la función, decidí eliminarlo ya que no era necesario tras realizar numerosas pruebas.

    // Para evitar bucles:
    if (contador_Acciones > MAX_ACCIONES)
    {
        // cout << "ENTRAAAAAAA";
        // Elegimos una acción aleatoria
        int posibleAccion = rand() % 3; // 0, 1 o 2
        if (posibleAccion == 0 && (sensores.superficie[1] == 'X' || sensores.superficie[1] == 'C'))
        {
            accion = TURN_SR;
            giro45Izq = 6;
        }
        else if (posibleAccion == 1 && (sensores.superficie[3] == 'X' || sensores.superficie[3] == 'C'))
        {
            accion = TURN_SR;
        }
        else if (posibleAccion == 2 && (sensores.superficie[2] == 'X' || sensores.superficie[2] == 'C'))
        {
            accion = WALK;
        }
    }

    // cout << "############AUXILIAR-FIN######################" << endl;

    // Devolver la siguiente acción a hacer
    last_action = accion;
    return accion;
}

// ════════════════════════════════════════════════════════════════════════════

// ════════════════════════════════════════════════════════════════════════════
//                              FUNCIONES NIVEL 1
// ════════════════════════════════════════════════════════════════════════════

pair<int, int> rotarA(int f, int c, int rumbo)
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

void SituarSensorEnMapaA(vector<vector<unsigned char>> &mapaTerreno, vector<vector<unsigned char>> &mapaAlturas, const Sensores &sensores)
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

int ComportamientoAuxiliar::VeoCasillaInteresanteNivel1(char i, char c, char d, bool zap)
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
    if (zap)
    { // Si tiene zapatillas, las casillas 'B' se comportan como 'C'
        if (c == 'C' || c == 'S' || c == 'B')
            return 2;
        else if (i == 'C' || i == 'S' || i == 'B')
            return 1;
        else if (d == 'C' || d == 'S' || d == 'B')
            return 3;
    }
    else
    {
        if (c == 'C' || c == 'S')
            return 2; // Caminos transitables
        else if (i == 'C' || i == 'S')
            return 1;
        else if (d == 'C' || d == 'S')
            return 3;
    }
    if (c == 'X')
        return 2; // Prioridad máxima: objetivo
    else if (i == 'X')
        return 1;
    else if (d == 'X')
        return 3;
    return 0; // Ninguna casilla interesante
}

bool ComportamientoAuxiliar::evitarColisionAuxiliar(const Sensores &sensores, Action &accion, int &giro45Izq)
{
    if (sensores.agentes[2] == 'a' || sensores.agentes[6] == 'a' || sensores.agentes[12] == 'a')
    {
        // cout << "Evito posible colisión con Auxiliar (cerca o delante+1)" << endl;
        accion = TURN_SR;
        giro45Izq = 3;
        return true;
    }
    return false;
}

bool ComportamientoAuxiliar::CasillVisitadamasde10veces(int f, int c)
{
    return vecesVisitadas[f][c] > 30;
}

bool ComportamientoAuxiliar::escasillavalida_lvl1(Sensores sensores, int n)
{
    return (sensores.superficie[n] == 'C' || sensores.superficie[n] == 'D' || sensores.superficie[n] == 'X' || sensores.superficie[n] == 'S') && (ViablePorAltura(sensores.superficie[n], sensores.cota[n] - sensores.cota[0]) != 'P');
}

vector<Action> ComportamientoAuxiliar::obtenerPosiblesMovimientos(const Sensores &sensores, int filaActual, int colActual)
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
            giro45Izq = 5;
            posiblesMovimientos.push_back(TURN_SR);
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
            giro45Izq = 5;
            posiblesMovimientos.push_back(TURN_SR);
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
            giro45Izq = 5;
            posiblesMovimientos.push_back(TURN_SR);
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
            giro45Izq = 5;
            posiblesMovimientos.push_back(TURN_SR);
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
            giro45Izq = 5;
            posiblesMovimientos.push_back(TURN_SR);
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
            giro45Izq = 5;
            posiblesMovimientos.push_back(TURN_SR);
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
            giro45Izq = 5;
            posiblesMovimientos.push_back(TURN_SR);
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
            giro45Izq = 5;
            posiblesMovimientos.push_back(TURN_SR);
        }
        else if (filaActual - 1 >= 0 && escasillavalida_lvl1(sensores, 3) && !visitadosBool[filaActual - 1][colActual])
        {
            posiblesMovimientos.push_back(TURN_SR);
        }
    }

    return posiblesMovimientos;
}

string traducirAccionA(Action accion)
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

// ════════════════════════════════════════════════════════════════════════════

// ════════════════════════════════════════════════════════════════════════════
//                              NIVEL 1
// ════════════════════════════════════════════════════════════════════════════

Action ComportamientoAuxiliar::ComportamientoAuxiliarNivel_1(Sensores sensores)
{
    Action accion;
    // Action last_action = IDLE;
    // cout << "############AUXILIAR-INICIO######################---iteracion: " << contador_iteraciones << endl;
    // cout << "El rumbo es: " << sensores.rumbo << endl;
    SituarSensorEnMapaA(mapaResultado, mapaCotas, sensores);
    vecesVisitadas[sensores.posF][sensores.posC]++;     // aumentamos el contador de veces visitadas de la casilla actual
    visitadosBool[sensores.posF][sensores.posC] = true; // marcamos la casilla como visitada
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
    contador++;
    contador_iteraciones++;
    vector<Action> posiblesMovimientos = obtenerPosiblesMovimientos(sensores, sensores.posF, sensores.posC);
    // Imprimir posibles movimientos
    // cout << "Posibles movimientos: ";
    for (const auto &movimiento : posiblesMovimientos)
    {
        // cout << traducirAccionA(movimiento) << ", ";
    }
    // cout << endl;

    if (sensores.energia == 1)
    {
        // cout << "Recargando energía" << endl;
        return IDLE; // Recargar energía
    }

    evitarColisionInicial = evitarColisionAuxiliar(sensores, accion, giro45Izq);

    // pintamos si esta evitando colisiones
    if (evitarColisionInicial)
    {
        // cout << "Evitar colisión con Rescatador Al inicio" << endl;
        return accion;
    }
    else
    {
        // cout << "No evitar colisión con Rescatador AL inicio" << endl;
    }

    int fila = sensores.posF;
    int col = sensores.posC;

    // Comprobamos si el agente tiene zapatillas
    if (sensores.superficie[0] == 'D')
    {
        tiene_zapatillas = true;
        // cout << "Entra en el if de las zapatillas" << endl;
    }

    // pintamos si tiene zapatillas o no
    if (tiene_zapatillas)
    {
        // cout << "Tiene zapatillas" << endl;
    }
    else
    {
        // cout << "No tiene zapatillas" << endl;
    }

    if (CasillVisitadamasde10veces(fila, col))
    {
        bool evitarColision = evitarColisionAuxiliar(sensores, accion, giro45Izq);
        if (evitarColision)
        {
            // cout << "Evitar colisión con Auxiliar" << endl;
            return accion;
        }
        else
        {
            // cout << "Entra en el if de la casilla visitada más de 10 veces" << endl;
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
                giro45Izq = 5;
                return TURN_SR;
            }
            else
            {
                // cout << "Entra en el else de la casilla visitada más de 10 veces" << endl;
                giro45Izq = 5;
                return TURN_SR;
            }
        }
    }

    if (sensores.agentes[2] == 'r' || sensores.agentes[6] == 'r' || sensores.agentes[12] == 'r')
    {
        // cout << "Evito posible colisión con Rescatador (cerca o delante+1)" << endl;
        giro45Izq = 3;           // Marca que está girando
        return accion = TURN_SR; // Gira a la derecha para evitar colisión
    }

    if (sensores.superficie[0] == 'D')
    {
        tiene_zapatillas = true;
        // cout << "Entra en el if de las zapatillas" << endl;
    }

    if (!posiblesMovimientos.empty())
    {
        // cout << "Entra en el if de posibles movimientos" << endl;
        // cout << "Va a salir" << endl;
        return posiblesMovimientos[0]; // Devolver el primer movimiento posible
    }

    if (giro45Izq != 0)
    { // Estoy haciendo un TURN_SL
        // cout << "Entra en el if de giro45Izq, ya que esta eliminado giros, la ultima acción es " << DescripcionAccion(last_action) << endl;
        accion = TURN_SR;
        giro45Izq--;
    }
    else
    {
        // cout << "Entra en el else de posibles movimientos" << endl;
        char i = ViablePorAltura(sensores.superficie[1], sensores.cota[1] - sensores.cota[0]);
        char c = ViablePorAltura(sensores.superficie[2], sensores.cota[2] - sensores.cota[0]);
        char d = ViablePorAltura(sensores.superficie[3], sensores.cota[3] - sensores.cota[0]);

        int pos = VeoCasillaInteresanteNivel1(i, c, d, tiene_zapatillas);
        switch (pos)
        {
        case 2: // Vamos a avanzar hacia delante
            // cout << "Entra en case 2::AUXILIAR" << endl;
            if (sensores.agentes[2] == 'r' || sensores.agentes[6] == 'r' || sensores.agentes[12] == 'r')
            {
                // cout << "Evito posible colisión con Rescatador (cerca o delante+1)" << endl;
                accion = TURN_SR; // Gira a la derecha para evitar colisión
                giro45Izq = 3;    // Marca que está girando
            }
            else
            {
                giro45Izq = 0;
                return WALK; // Avanzar
            }
            break;
        case 1:
            // cout << "Entra en case 1::Auxiliar" << endl;
            giro45Izq = 6; // gira 5 veces 45º a la derecha = 270º = mirar hacia la izquierda
            // accion = TURN_L;
            accion = TURN_SR;
            break;
        case 3:
            // cout << "Entra en case 3::Auxiliar" << endl;
            accion = TURN_SR;
            break;
        case 0:
            // cout << "Entra en case 0::Auxiliar" << endl;
            giro45Izq = 5;
            accion = TURN_SR;
            // hay veces en las que entra aqui de manera indefinida por ello vamos a implementar una manera de que salga de este bucle
            break;
        }
    }

    // Para evitar bucles:
    if (contador_Acciones > MAX_ACCIONES)
    {
        // cout << "ENTRAAAAAAA";
        // Elegimos una acción aleatoria
        int posibleAccion = rand() % 3; // 0, 1 o 2
        if (posibleAccion == 0 && (sensores.superficie[1] == 'X' || sensores.superficie[1] == 'C'))
        {
            accion = TURN_SR;
            giro45Izq = 6;
        }
        else if (posibleAccion == 1 && (sensores.superficie[3] == 'X' || sensores.superficie[3] == 'C'))
        {
            accion = TURN_SR;
        }
        else if (posibleAccion == 2 && (sensores.superficie[2] == 'X' || sensores.superficie[2] == 'C'))
        {
            accion = WALK;
        }
    }

    // cout << "############AUXILIAR-FIN######################" << endl;

    // Devolver la siguiente acción a hacer
    last_action = accion;
    return accion;
}

// ════════════════════════════════════════════════════════════════════════════

// ════════════════════════════════════════════════════════════════════════════
//                              FUNCIONES NIVEL E
// ════════════════════════════════════════════════════════════════════════════

void AnularMatrizA(vector<vector<unsigned char>> &m)
{
    for (int i = 0; i < m.size(); i++)
    {
        for (int j = 0; j < m[i].size(); j++)
        {
            m[i][j] = 0;
        }
    }
}

bool ComportamientoAuxiliar::Find(const NodoA &st, const list<NodoA> &lista)
{
    auto it = lista.begin();
    while (it != lista.end() and !((*it) == st))
    {
        it++;
    }
    return (it != lista.end());
}

void ComportamientoAuxiliar::VisualizaPlan(const EstadoA &st, vector<Action> &plan)
{
    AnularMatrizA(mapaConPlan);
    EstadoA cst = st;
    auto it = plan.begin();
    while (it != plan.end())
    {
        switch (*it)
        {
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
            mapaConPlan[cst.f][cst.c] = 2;
            break;
        case TURN_SR:
            cst.brujula = (cst.brujula + 1) % 8;
            break;
        }
        it++;
    }
}

EstadoA ComportamientoAuxiliar::NextCasillaAuxiliar(const EstadoA &st)
{
    EstadoA siguiente = st;
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
    if (siguiente.f < 0 || siguiente.c < 0 || siguiente.f > mapaResultado.size() || siguiente.c > mapaResultado.size())
        siguiente = st;
    return siguiente;
}

bool ComportamientoAuxiliar::CasillaAccesibleAuxiliar(const EstadoA &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura)
{
    EstadoA next = NextCasillaAuxiliar(st);
    bool check1 = false, check2 = false, check3 = false, check4 = false;
    // Comprobamos que la casilla está dentro de los límites del mapa
    if (next.f >= 0 && next.f < terreno.size() &&
        next.c >= 0 && next.c < terreno[0].size())
    {

        check4 = true;

        check1 = terreno[next.f][next.c] != 'P' && terreno[next.f][next.c] != 'M';
        check2 = terreno[next.f][next.c] != 'B' || (terreno[next.f][next.c] == 'B' && st.zapatillas);
        check3 = abs(altura[next.f][next.c] - altura[st.f][st.c]) <= 1;

        if (terreno[next.f][next.c] == '?')
            check3 = true;
    }

    return check1 && check2 && check3 && check4;
}

EstadoA ComportamientoAuxiliar::applyA(Action accion, const EstadoA &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura)
{
    EstadoA next = st;
    switch (accion)
    {
    case WALK:
        if (CasillaAccesibleAuxiliar(st, terreno, altura))
        {
            next = NextCasillaAuxiliar(st);
        }
        break;
    case TURN_SR:
        next.brujula = (next.brujula + 1) % 8;
        break;
    }
    return next;
}

// ════════════════════════════════════════════════════════════════════════════
//                             NIVEL 2
// ════════════════════════════════════════════════════════════════════════════

Action ComportamientoAuxiliar::ComportamientoAuxiliarNivel_2(Sensores sensores)
{

    return IDLE; // Comportamiento auxiliar para el nivel 2, en este no hace nada
}

// ════════════════════════════════════════════════════════════════════════════

// ════════════════════════════════════════════════════════════════════════════
//                              FUNCIONES NIVEL 3
// ════════════════════════════════════════════════════════════════════════════

int ComportamientoAuxiliar::CosteBaseTerreno(unsigned char casilla, Action accion)
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

int ComportamientoAuxiliar::CosteCambioAltura(int altura_origen, int altura_destino, unsigned char casilla, Action accion)
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
    default:
        return 0;
    }
}
// En el A estrella lo cambiamos por vector<Action> ya que este tipo de estructura es más eficiente

vector<Action> ComportamientoAuxiliar::AEstrellaAuxiliar(const EstadoA &origen, const EstadoA &destino,
                                                         const vector<vector<unsigned char>> &terreno,
                                                         const vector<vector<unsigned char>> &altura)
{
    NodoA current_nodo;
    priority_queue<NodoA, vector<NodoA>, greater<NodoA>> frontier;
    set<NodoA> explored;
    vector<Action> path;

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

    // Inicializamos con el coste estimado desde el origen
    int heuristica_origen = max(abs(destino.f - origen.f), abs(destino.c - origen.c));
    costeMinimo[origen.f][origen.c][origen.brujula][origen.zapatillas ? 1 : 0] = heuristica_origen;

    while (!frontier.empty())
    {
        current_nodo = frontier.top();
        frontier.pop();

        if (current_nodo.estado.f == destino.f && current_nodo.estado.c == destino.c)
        {
            return current_nodo.secuencia;
        }

        explored.insert(current_nodo);

        if (terreno[current_nodo.estado.f][current_nodo.estado.c] == 'D')
        {
            current_nodo.estado.zapatillas = true;
        }

        vector<Action> acciones = {WALK, TURN_SR};

        for (Action accion : acciones)
        {
            NodoA hijo = current_nodo;
            hijo.estado = applyA(accion, current_nodo.estado, terreno, altura);

            int f = hijo.estado.f;
            int c = hijo.estado.c;
            if (f < 0 || f >= filas || c < 0 || c >= columnas)
                continue;

            int costeTerreno = CosteBaseTerreno(terreno[current_nodo.estado.f][current_nodo.estado.c], accion);
            int costeAltura = (accion == WALK || accion == RUN) ? CosteCambioAltura(
                                                                      altura[current_nodo.estado.f][current_nodo.estado.c],
                                                                      altura[hijo.estado.f][hijo.estado.c],
                                                                      terreno[current_nodo.estado.f][current_nodo.estado.c],
                                                                      accion)
                                                                : 0;

            int costeAccion = costeTerreno + costeAltura;
            hijo.coste = current_nodo.coste + costeAccion;
            hijo.secuencia = current_nodo.secuencia;
            hijo.secuencia.push_back(accion);

            // Heurística Chebyshev
            int heuristica = max(abs(destino.f - hijo.estado.f), abs(destino.c - hijo.estado.c));
            int costeEstimado = hijo.coste + heuristica;

            int ori = hijo.estado.brujula;
            int zap = hijo.estado.zapatillas ? 1 : 0;

            // ⬇️ Ahora comparamos el coste estimado total (coste + heurística)
            if (costeEstimado < costeMinimo[f][c][ori][zap])
            {
                costeMinimo[f][c][ori][zap] = costeEstimado;
                hijo.prioridad = costeEstimado;
                frontier.push(hijo);
            }
        }
    }

    cout << "No se encontró un camino al destino (A* auxiliar con heurística Chebyshev)." << endl;
    return path;
}

void ComportamientoAuxiliar::PintaPlan(vector<Action> &plan, bool zap)
{
    auto it = plan.begin();
    while (it != plan.end())
    {
        if (*it == WALK)
        {
            cout << "W ";
        }
        else if (*it == TURN_SR)
        {
            cout << "r ";
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

// ════════════════════════════════════════════════════════════════════════════

// ════════════════════════════════════════════════════════════════════════════
//                             NIVEL 3
// ════════════════════════════════════════════════════════════════════════════

Action ComportamientoAuxiliar::ComportamientoAuxiliarNivel_3(Sensores sensores)
{
    Action accion = IDLE;
    if (!hayPlan)
    {
        // Invocar al método de búsqueda
        EstadoA inicio, fin;
        inicio.f = sensores.posF;
        inicio.c = sensores.posC;
        inicio.brujula = sensores.rumbo;
        // cout << "El valor de tiene_zapatillas antes de asignar es: " << tiene_zapatillas << endl;
        inicio.zapatillas = tiene_zapatillas;
        // cout << "En el nivel 2 el valor de tiene_zapatillas es: " << tiene_zapatillas << endl;
        fin.f = sensores.destinoF;
        fin.c = sensores.destinoC;
        plan = AEstrellaAuxiliar(inicio, fin, mapaResultado, mapaCotas);
        // pintamos el plan
        // cout << "Plan encontrado: ";
        PintaPlan(plan, tiene_zapatillas);
        VisualizaPlan(inicio, plan);
        hayPlan = !plan.empty();
    }
    if (hayPlan && !plan.empty())
    {
        accion = plan.front();
        plan.erase(plan.begin());
    }
    if (plan.empty())
    {
        hayPlan = false;
    }

    return accion;
}

// ════════════════════════════════════════════════════════════════════════════
//                              FUNCIONES NIVEL 4
// ════════════════════════════════════════════════════════════════════════════

vector<Action> ComportamientoAuxiliar::AEstrellaAuxiliar_lvl4(const EstadoA &origen, EstadoA &destino, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura, bool esparaRecargar = false)
{
    NodoA current_nodo;
    priority_queue<NodoA, vector<NodoA>, greater<NodoA>> frontier;
    set<NodoA> explored;
    vector<Action> path;

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

        // para los niveles S del 4----------------
        if (!esparaRecargar)
        {
            if (current_nodo.estado.brujula == noroeste || current_nodo.estado.brujula == suroeste || current_nodo.estado.brujula == noreste || current_nodo.estado.brujula == sureste)
            {
                if (abs(current_nodo.estado.f - destino.f) <= 3 && abs(current_nodo.estado.c - destino.c) <= 3)
                {
                    destino.f = current_nodo.estado.f;
                    destino.c = current_nodo.estado.c;
                }
            }
        }
        // para los niveles S del 4----------------

        if (current_nodo.estado.f == destino.f && current_nodo.estado.c == destino.c)
        {
            return current_nodo.secuencia; // Devuelve vector<Action>
        }

        explored.insert(current_nodo);

        if (terreno[current_nodo.estado.f][current_nodo.estado.c] == 'D')
        {
            current_nodo.estado.zapatillas = true;
        }

        vector<Action> acciones = {WALK, TURN_SR};

        for (Action accion : acciones)
        {
            NodoA hijo = current_nodo;
            hijo.estado = applyA(accion, current_nodo.estado, terreno, altura);

            int f = hijo.estado.f;
            int c = hijo.estado.c;
            if (f < 0 || f >= filas || c < 0 || c >= columnas)
                continue;

            int costeTerreno = CosteBaseTerreno(terreno[current_nodo.estado.f][current_nodo.estado.c], accion);
            int costeAltura = (accion == WALK || accion == RUN) ? CosteCambioAltura(
                                                                      altura[current_nodo.estado.f][current_nodo.estado.c],
                                                                      altura[hijo.estado.f][hijo.estado.c],
                                                                      terreno[current_nodo.estado.f][current_nodo.estado.c],
                                                                      accion)
                                                                : 0;

            int costeAccion = costeTerreno + costeAltura;
            hijo.coste = current_nodo.coste + costeAccion;
            hijo.secuencia = current_nodo.secuencia;
            hijo.secuencia.push_back(accion);

            int heuristica = abs(destino.f - hijo.estado.f) + abs(destino.c - hijo.estado.c);
            int costeEstimado = hijo.coste + heuristica;

            int ori = hijo.estado.brujula;
            int zap = hijo.estado.zapatillas ? 1 : 0;

            if (hijo.coste < costeMinimo[f][c][ori][zap])
            {
                costeMinimo[f][c][ori][zap] = hijo.coste;
                hijo.prioridad = costeEstimado;
                frontier.push(hijo);
            }
        }
    }

    // cout << "No se encontró un camino al destino (A* auxiliar)." << endl;
    return path; // Retorna vector<Action> vacío si no hay solución
}

bool ComportamientoAuxiliar::casillValidad_lvl4(Sensores sensores, int pos, bool tiene_zapatillas)
{
    if (sensores.posF >= 0 && sensores.posF < mapaResultado.size() && sensores.posC >= 0 && sensores.posC < mapaResultado[0].size() &&
        sensores.superficie[pos] != 'M' && sensores.superficie[pos] != 'P' && ViablePorAltura(sensores.superficie[pos], sensores.cota[pos] - sensores.cota[0]) != 'P')
    {
        return true;
    }
    else
    {
        return false;
    }
}

Action ComportamientoAuxiliar::comportamientoReactivo(Sensores sensores, int &giro45Izq)
{
    Action accion = IDLE;
    bool delante = casillValidad_lvl4(sensores, 2, tiene_zapatillas);
    bool derecha = casillValidad_lvl4(sensores, 3, tiene_zapatillas);
    bool izquierda = casillValidad_lvl4(sensores, 1, tiene_zapatillas);
    // cout << "El valor de delante es: " << delante << endl;
    // cout << "El valor de derecha es: " << derecha << endl;
    // cout << "El valor de izquierda es: " << izquierda << endl;
    // cout << "El valor de tiene_zapatillas es: " << tiene_zapatillas << endl;

    // Prioritize 'C' casilla
    if (sensores.superficie[2] == 'C' && delante)
    {
        // cout << "Avanzando hacia delante (priorizando casilla C) para salir del apuro" << endl;
        accion = WALK;
    }
    else if (sensores.superficie[3] == 'C' && derecha)
    {
        // cout << "Girando suavemente a la derecha (priorizando casilla C) para salir del apuro" << endl;
        accion = TURN_SR;
    }
    else if (sensores.superficie[1] == 'C' && izquierda)
    {
        // cout << "Girando suavemente a la izquierda (priorizando casilla C) para salir del apuro" << endl;
        giro45Izq = 6;
        accion = TURN_SR;
    }
    else if (derecha && izquierda && delante)
    {
        int randomValue = rand() % 3;
        if (randomValue == 0)
        {
            // cout << "Avanzando hacia delante para salir del apuro" << endl;
            accion = WALK;
        }
        else if (randomValue == 1)
        {
            // cout << "Girando suavemente a la derecha para salir del apuro" << endl;
            accion = TURN_SR;
        }
        else
        {
            // cout << "Girando suavemente a la izquierda para salir del apuro" << endl;
            giro45Izq = 6;
            accion = TURN_SR;
        }
    }
    else if (derecha && izquierda)
    {
        int randomValue = rand() % 2;
        if (randomValue == 0)
        {
            // cout << "Girando suavemente a la derecha para salir del apuro" << endl;
            accion = TURN_SR;
        }
        else
        {
            // cout << "Girando suavemente a la izquierda para salir del apuro" << endl;
            giro45Izq = 6;
            accion = TURN_SR;
        }
    }
    else if (derecha && delante)
    {
        int randomValue = rand() % 2;
        if (randomValue == 0)
        {
            // cout << "Avanzando hacia delante para salir del apuro" << endl;
            accion = WALK;
        }
        else
        {
            // cout << "Girando suavemente a la izquierda para salir del apuro" << endl;
            giro45Izq = 6;
            accion = TURN_SR;
        }
    }
    else if (izquierda && delante)
    {
        int randomValue = rand() % 2;
        if (randomValue == 0)
        {
            // cout << "Avanzando hacia delante para salir del apuro" << endl;
            accion = WALK;
        }
        else
        {
            // cout << "Girando suavemente a la derecha para salir del apuro" << endl;
            accion = TURN_SR;
        }
    }
    else if (derecha)
    {
        // cout << "Girando suavemente a la izquierda para salir del apuro" << endl;
        giro45Izq = 6;
        accion = TURN_SR;
    }
    else if (izquierda)
    {
        // cout << "Girando suavemente a la derecha para salir del apuro" << endl;
        accion = TURN_SR;
    }
    else if (delante)
    {
        // cout << "Avanzando hacia delante para salir del apuro" << endl;
        accion = WALK;
    }
    else
    {
        // cout << "No hay salida, girando suavemente a la derecha para salir del apuro" << endl;
        giro45Izq = 5;
        accion = TURN_SR;
    }

    return accion;
}

bool casillaTransitable(unsigned char terreno, bool tiene_zapatillas)
{
    return terreno == 'C' || terreno == 'S' || terreno == 'T' || terreno == 'A' || (tiene_zapatillas) ? terreno == 'B' : terreno != 'M' && terreno != 'P';
}

int contarInterrogantesAlrededor_A(const vector<vector<unsigned char>> &mapa, int f, int c)
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
priority_queue<CasillaDescubrimiento_Auxiliar> ComportamientoAuxiliar::cola_con_casillas_con_interrogacion(Sensores sensores)
{
    priority_queue<CasillaDescubrimiento_Auxiliar> explorables;

    for (int f = 1; f < mapaResultado.size() - 1; ++f)
    {
        for (int c = 1; c < mapaResultado[0].size() - 1; ++c)
        {
            if (!visitadosCasillasInterrogacion[f][c])
            {
                if (!casillaTransitable(mapaResultado[f][c], tiene_zapatillas))
                    continue;

                int numInterrogantes = contarInterrogantesAlrededor_A(mapaResultado, f, c);
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

// ════════════════════════════════════════════════════════════════════════════
//                             NIVEL 4
// ════════════════════════════════════════════════════════════════════════════
//

Action ComportamientoAuxiliar::ComportamientoAuxiliarNivel_4(Sensores sensores)
{
    Action accion = IDLE;
    int valorRecarga = 2000;
    SituarSensorEnMapaA(mapaResultado, mapaCotas, sensores);
    int cuandoRecargar = 1000;

    if (mapaResultado.size() == 100)
    {
        valorRecarga = 3000;
    }

    // cout << "############AUXILIAR-NIVEL-4-INICIO######################" << endl;

    contador++; // para depurar
    // cout << "El contador es: " << contador << endl;
    // cout << "Sensores. tiempo: " << sensores.tiempo << endl;

    if (giro45Izq > 0)
    {
        // cout << "Entra en el if de giro45Izq, ya que esta eliminado giros " << endl;
        giro45Izq--;
        return TURN_SR;
    }

    if (sensores.superficie[0] == 'D')
    {
        tiene_zapatillas = true;
    }

    // POSICION -----------------

    pair<int, int> posicionAuxiliar = make_pair(sensores.posF, sensores.posC);
    if (posicionAuxiliar == lastPosition)
    {
        contadorPosicionesIguales++;
    }
    else
    {
        contadorPosicionesIguales = 0;
    }
    lastPosition = posicionAuxiliar;

    // POSICION -----------------

    // RECARGA DE ENERGIA--------------
    // cout << "El sensores.energia es: " << sensores.energia << endl;
    if (sensores.energia >= valorRecarga && recargaEnergia)
    {
        // cout << "Entra en el if de la energia == 1500" << endl;
        recargaEnergia = false; // si la energia es 3000, quiere decir que ya hemos recargado
    }

    if (sensores.energia < cuandoRecargar && !sensores.venpaca)
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
            EstadoA inicio, fin;
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

            plan = AEstrellaAuxiliar_lvl4(inicio, fin, mapaResultado, mapaCotas, true);
            VisualizaPlan(inicio, plan);
            hayPlan = plan.size() != 0;
        }
        else
        {
            // cout << "Entra en el else de listaPuestosRecarga.size() > 0" << endl;
            // no hay puestos de recarga, por lo que no podemos hacer nada
        }
    }
    // cout << "El valor de estoyRecargando es: " << estoyRecargando << endl;
    // cout << "El valor de recargaEnergia es: " << recargaEnergia << endl;
    if (recargaEnergia && estoyRecargando)
    {
        return IDLE;
    }

    // RECARGA DE ENERGIA--------------

    if (sensores.venpaca)
    {

        // BUSCAMOS DESTINO ----------------

        // si el mapa es de tamaño 30 y no tengo zapatillas y es el primer turno, voy al centro que estan las zapatillas
        // si la llamada pertenece de una de las esquinas del mapa de 30x30, booleano a true

        bool centroVacio = (mapaResultado[15][15] == '?') ? true : false;
        if (mapaResultado.size() == 30 && !tiene_zapatillas && centroVacio)
        {
            // cout << "Entra en el if de mapaResultado.size() == 30 && !tiene_zapatillas && esPrimerTurno" << endl;
            EstadoA inicio, fin;
            inicio.f = sensores.posF;
            inicio.c = sensores.posC;
            inicio.brujula = sensores.rumbo;
            inicio.zapatillas = tiene_zapatillas;
            fin.f = 15;
            fin.c = 15;
            plan = AEstrellaAuxiliar_lvl4(inicio, fin, mapaResultado, mapaCotas);
            VisualizaPlan(inicio, plan);
            hayPlan = plan.size() != 0;
        }

        if (!hayPlan)
        {

            EstadoA inicio, fin;
            inicio.f = sensores.posF;
            inicio.c = sensores.posC;
            inicio.brujula = sensores.rumbo;
            inicio.zapatillas = tiene_zapatillas;
            fin.f = sensores.destinoF;
            fin.c = sensores.destinoC;
            plan = AEstrellaAuxiliar_lvl4(inicio, fin, mapaResultado, mapaCotas);
            VisualizaPlan(inicio, plan);
            hayPlan = plan.size() != 0;
            // cout << "Plan encontrado de tamaño: " << plan.size() << endl;
            PintaPlan(plan, tiene_zapatillas);
        }

        // BUSCAMOS DESTINO ----------------

        // SACAMOS LAS ACCIONES DEL PLAN--------

        // SI EL PLAN ES 0 PONEMOS EL HAYPLAN A FALSE

        if (plan.size() == 0)
        {
            // cout << "Entra en el if de plan.size() == 0" << endl;
            hayPlan = false;
            plan.clear();

            if (contadorPosicionesIguales > 10)
            {
                priority_queue<CasillaDescubrimiento_Auxiliar> cola = cola_con_casillas_con_interrogacion(sensores);

                if (!cola.empty())
                {
                    // cout << "Entra en el if de cola.size() > 0" << endl;
                    // Plan hacia la casilla de interrogación más cercana
                    EstadoA inicio, fin;
                    inicio.f = sensores.posF;
                    inicio.c = sensores.posC;
                    inicio.brujula = sensores.rumbo;
                    inicio.zapatillas = tiene_zapatillas;
                    fin.f = cola.top().f;
                    fin.c = cola.top().c;
                    plan = AEstrellaAuxiliar_lvl4(inicio, fin, mapaResultado, mapaCotas);
                    VisualizaPlan(inicio, plan);
                    hayPlan = plan.size() != 0;
                }
                else
                {

                    vector<Action> acciones = {WALK, TURN_SR};
                    int randomValue = rand() % acciones.size();
                    accion = acciones[randomValue];
                }
            }
        }
    }

    // SI hay agua y la energia es menor de 500:
    if (sensores.energia <= 500 && mapaResultado.size() >= 75 && hayPlan)
    {
        vector<vector<unsigned char>> mapaAux = mapaResultado;
        for (int i = 0; i < mapaResultado.size(); i++)
        {
            for (int j = 0; j < mapaResultado[0].size(); j++)
            {
                if (mapaAux[i][j] == 'A')
                {
                    mapaAux[i][j] = 'M';
                }
            }
        }

        EstadoA inicio, fin;
        inicio.f = sensores.posF;
        inicio.c = sensores.posC;
        inicio.brujula = sensores.rumbo;
        inicio.zapatillas = tiene_zapatillas;
        fin.f = sensores.destinoF;
        fin.c = sensores.destinoC;
        plan = AEstrellaAuxiliar_lvl4(inicio, fin, mapaAux, mapaCotas);
        VisualizaPlan(inicio, plan);
        hayPlan = plan.size() != 0;
        // cout << "Plan encontrado de tamaño: " << plan.size() << endl;
    }

    if (hayPlan && plan.size() > 0)
    {
        // cout << "Entra en el if de hayPlan y el tamaño de plan es > 0" << endl;
        // cout << "La pos actual es: " << sensores.posF << " " << sensores.posC << endl;

        accion = plan.front();
        plan.erase(plan.begin());
    }

    if (accion == WALK)
    {
        // cout << "Entra en el if de accion == WALK" << endl;
        EstadoA actual = {sensores.posF, sensores.posC, sensores.rumbo, tiene_zapatillas};
        EstadoA siguiente = applyA(accion, actual, mapaResultado, mapaCotas);
        if (actual == siguiente)
        {
            hayPlan = false;
            plan.clear();
            accion = IDLE;
        }
    }

    // COMPROBACIÓN ADICIONAL---------------------------

    // cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@AUXILIAR-FIN@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
    return accion;
}

// ════════════════════════════════════════════════════════════════════════════