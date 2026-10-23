#ifndef CACHE_H
#define CACHE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define TAM_BLOQUE 4   
#define NUM_BLOQUES 16 

typedef struct {
    bool valido;
    size_t tag;
    uint8_t datos[TAM_BLOQUE];
    size_t timestamp_fifo;
} BloqueCache;

typedef struct {
    BloqueCache bloques[NUM_BLOQUES];
    size_t hits;
    size_t misses;
    size_t contador_fifo;
} Cache;

void inicializar_cache(Cache *cache);
bool buscar_en_cache(Cache *cache, size_t direccion, uint8_t *buffer, size_t n_bytes);
void insertar_en_cache(Cache *cache, size_t direccion, const uint8_t *datos_bloque);
void invalidar_cache(Cache *cache);
void obtener_estadisticas(const Cache *cache, size_t *hits, size_t *misses, double *tasa);

#endif
