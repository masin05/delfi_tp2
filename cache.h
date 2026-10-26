#ifndef CACHE_H
#define CACHE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "memoria.h"

#define PALABRAS_POR_BLOQUE 4
#define TAM_BLOQUE (PALABRAS_POR_BLOQUE * TAM_PALABRA) // bytes por bloque
#define NUM_BLOQUES 16 // capacidad de la cache, en bloques

typedef struct {
    bool valido;
    size_t numero_bloque; // que bloque de la memoria contiene (dirección / TAM_BLOQUE)
    size_t orden_ingreso; // cuando entro; se usa para elegir a quién descartar (FIFO)
    uint8_t datos[TAM_BLOQUE];
} BloqueCache;

// la cache solo guarda bloques, no conoce la memoria principal
typedef struct {
    BloqueCache bloques[NUM_BLOQUES];
    size_t contador_ingreso;
} Cache;

void cache_inicializar(Cache *cache);

// devuelve el bloque si está en la cache, o null si no esta
BloqueCache *cache_buscar(Cache *cache, size_t numero_bloque);

// guarda un bloque nuevo. si hay un lugar vacío lo usa, si no, descarta el bloque que entro primero. devuelve el bloque donde quedo guardado.
BloqueCache *cache_insertar(Cache *cache, size_t numero_bloque, const uint8_t *datos_bloque);

// vacía la cache (todos los bloques pasan a invalidos)
void cache_invalidar(Cache *cache);

#endif
