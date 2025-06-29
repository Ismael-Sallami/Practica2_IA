# Práctica 2 - Inteligencia Artificial (UGR)

Este repositorio contiene la implementación de los agentes **Rescatador** y **Auxiliar**, diseñados con comportamientos tanto reactivos como deliberativos para un entorno simulado de rescate. El proyecto corresponde a la asignatura de Inteligencia Artificial del Grado en Ingeniería Informática en la Universidad de Granada (curso 2024-2025).

## 👨‍💻 Autor

Ismael Sallami Moreno  
Estudiante del Doble Grado en Ingeniería Informática y ADE  
Universidad de Granada

## 🧠 Objetivo

Desarrollar agentes inteligentes capaces de colaborar en un equipo de rescate virtual, actuando sobre un mapa dinámico y enfrentándose a distintos niveles de dificultad:

- **Niveles 0 y 1**: Agentes con comportamiento reactivo.
- **Niveles 2 y 3**: Agentes deliberativos utilizando algoritmos de búsqueda (Dijkstra, A*).
- **Nivel 4**: Integración de comportamientos reactivos y deliberativos.

## 📁 Estructura de entrega

Los únicos archivos que deben modificarse y entregarse son:

- `rescatador.hpp`
- `rescatador.cpp`
- `auxiliar.hpp`
- `auxiliar.cpp`

> Todos estos archivos deben incluirse en un archivo `.zip` llamado `practica2.zip` para su entrega en PRADO.

## ⚠️ Aviso

La práctica es individual y estará sujeta a revisión para evitar plagio.

## 🛠️ Instalación

Consulta el repositorio oficial del simulador:  
https://github.com/ugr-ccia-IA/practica2

Sigue las instrucciones del script `install.sh` para compilar en Linux (preferiblemente Ubuntu).

> *El desarrollo se realizó en el directorio `IA_practica2`, mientras que `practica2` corresponde a la versión entregada.* Se recomienda visitar el blog [elblogdeismael.github.io](https://elblogdeismael.github.io) para consultar otras prácticas de inteligencia artificial.