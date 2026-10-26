#ifndef MEMORIA_H
#define MEMORIA_H

#include "cache.h"

#ifndef PALABRAS_MEMORIA
#define PALABRAS_MEMORIA 1024 // capacidad de la memoria principal, en palabras
#endif

#define TAM_MEMORIA (PALABRAS_MEMORIA * TAM_PALABRA)

// memoria principal (secuencia de palabras direccionables) con su caché delante
// política de escritura: write-through (la memoria siempre está al día)
typedef struct {
    uint8_t datos[TAM_MEMORIA];
    bool palabra_en_uso[PALABRAS_MEMORIA]; // true si la palabra fue escrita, lo usa la compactación
    Cache cache;
    size_t hits;
    size_t misses;
} Memoria;

typedef struct {
    size_t hits;
    size_t misses;
    double tasa_efectividad; // hits / (hits + misses), o 0 si todavía no hubo accesos
} Estadisticas;

void memoria_inicializar(Memoria *memoria);

// lee n_bytes a partir de `direccion` pasando por la caché
// se accede a palabras completas y cada palabra tocada cuenta como un acceso (un hit o un miss)
// devuelve false, sin hacer nada, si el rango se sale de la memoria
bool memoria_leer(Memoria *memoria, size_t direccion, size_t n_bytes, uint8_t *buffer);

// escribe n_bytes a partir de `direccion` en la memoria y en la caché a la vez
bool memoria_escribir(Memoria *memoria, size_t direccion, size_t n_bytes, const uint8_t *datos);

// junta las palabras en uso al principio de la memoria, sin cambiar su orden, para eliminar los huecos
// las direcciones de los datos cambian y se vacía la caché porque quedó desactualizada
void memoria_compactar(Memoria *memoria);

Estadisticas memoria_obtener_estadisticas(const Memoria *memoria);

#endif
