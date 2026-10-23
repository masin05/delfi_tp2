#include "cache.h"
#include <string.h>

void inicializar_cache(Cache *cache) {
    for (int i = 0; i < NUM_BLOQUES; i++) {
        cache->bloques[i].valido = false;
        cache->bloques[i].tag = 0;
        cache->bloques[i].timestamp_fifo = 0;
        memset(cache->bloques[i].datos, 0, TAM_BLOQUE);
    }
    cache->hits = 0;
    cache->misses = 0;
    cache->contador_fifo = 0;
}

bool buscar_en_cache(Cache *cache, size_t direccion, uint8_t *buffer, size_t n_bytes) {
    size_t numero_bloque = direccion / TAM_BLOQUE;
    size_t offset = direccion % TAM_BLOQUE;

    for (int i = 0; i < NUM_BLOQUES; i++) {
        if (cache->bloques[i].valido && cache->bloques[i].tag == numero_bloque) {
            cache->hits++;
            size_t bytes_a_copiar = (n_bytes < (TAM_BLOQUE - offset)) ? n_bytes : (TAM_BLOQUE - offset);
            memcpy(buffer, cache->bloques[i].datos + offset, bytes_a_copiar);
            return true;
        }
    }

    cache->misses++;
    return false;
}

void insertar_en_cache(Cache *cache, size_t direccion, const uint8_t *datos_bloque) {
    size_t numero_bloque = direccion / TAM_BLOQUE;
    int indice_reemplazo = -1;

    for (int i = 0; i < NUM_BLOQUES; i++) {
        if (!cache->bloques[i].valido) {
            indice_reemplazo = i;
            break;
        }
    }

    if (indice_reemplazo == -1) {
        size_t min_fifo = cache->bloques[0].timestamp_fifo;
        indice_reemplazo = 0;
        for (int i = 1; i < NUM_BLOQUES; i++) {
            if (cache->bloques[i].timestamp_fifo < min_fifo) {
                min_fifo = cache->bloques[i].timestamp_fifo;
                indice_reemplazo = i;
            }
        }
    }

    cache->bloques[indice_reemplazo].valido = true;
    cache->bloques[indice_reemplazo].tag = numero_bloque;
    memcpy(cache->bloques[indice_reemplazo].datos, datos_bloque, TAM_BLOQUE);
    cache->bloques[indice_reemplazo].timestamp_fifo = cache->contador_fifo++;
}

void invalidar_cache(Cache *cache) {
    for (int i = 0; i < NUM_BLOQUES; i++) {
        cache->bloques[i].valido = false;
    }
}

void obtener_estadisticas(const Cache *cache, size_t *hits, size_t *misses, double *tasa) {
    *hits = cache->hits;
    *misses = cache->misses;
    size_t total = cache->hits + cache->misses;
    *tasa = (total == 0) ? 0.0 : (double)cache->hits / total;
}
