#ifndef CACHE_H
#define CACHE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef TAM_PALABRA
#define TAM_PALABRA 4 // bytes por palabra
#endif
#ifndef PALABRAS_POR_BLOQUE
#define PALABRAS_POR_BLOQUE 4 // palabras consecutivas que se copian en cada miss
#endif
#ifndef NUM_BLOQUES
#define NUM_BLOQUES 16 // capacidad de la caché, en bloques
#endif

#define TAM_BLOQUE (PALABRAS_POR_BLOQUE * TAM_PALABRA) // bytes por bloque

typedef struct {
    bool valido;
    size_t numero_bloque; // que bloque de la memoria contiene (índice de palabra / PALABRAS_POR_BLOQUE)
    uint8_t datos[TAM_BLOQUE];
} BloqueCache;

// la cache solo guarda bloques, no conoce la memoria principal
typedef struct {
    BloqueCache bloques[NUM_BLOQUES];
    size_t proximo_a_reemplazar; // posicion del bloque más viejo (FIFO)
} Cache;

void cache_inicializar(Cache *cache);

// lee una palabra de la cache
// devuelve true (hit) y copia la palabra en "palabra" si su bloque está en la cache
// devuelve false (miss) si no está
bool cache_leer(Cache *cache, size_t indice_palabra, uint8_t palabra[TAM_PALABRA]);

// guarda un bloque completo, si ya estaba lo sobrescribe
// si no estaba y la cache esta llena, reemplaza el bloque que entro primero (FIFO)
void cache_escribir(Cache *cache, size_t numero_bloque, const uint8_t datos_bloque[TAM_BLOQUE]);

// vacia la cache: todos los bloques pasan a inválidos
void cache_invalidar(Cache *cache);

#endif
