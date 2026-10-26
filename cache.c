#include "cache.h"

#include <string.h>

void cache_inicializar(Cache *cache) {
    memset(cache, 0, sizeof *cache); // todos los bloques quedan inválidos
}

BloqueCache *cache_buscar(Cache *cache, size_t numero_bloque) {
    for (size_t i = 0; i < NUM_BLOQUES; i++) {
        if (cache->bloques[i].valido && cache->bloques[i].numero_bloque == numero_bloque) {
            return &cache->bloques[i];
        }
    }
    return NULL;
}

BloqueCache *cache_insertar(Cache *cache, size_t numero_bloque, const uint8_t *datos_bloque) {
    BloqueCache *destino = &cache->bloques[0];

    for (size_t i = 0; i < NUM_BLOQUES; i++) {
        BloqueCache *candidato = &cache->bloques[i];
        if (!candidato->valido) {
            destino = candidato; // hay un lugar vacío: no hace falta descartar nada
            break;
        }
        if (candidato->orden_ingreso < destino->orden_ingreso) {
            destino = candidato; //hasta ahora es el que entró primero
        }
    }

    destino->valido = true;
    destino->numero_bloque = numero_bloque;
    destino->orden_ingreso = cache->contador_ingreso++;
    memcpy(destino->datos, datos_bloque, TAM_BLOQUE);
    return destino;
}

void cache_invalidar(Cache *cache) {
    for (size_t i = 0; i < NUM_BLOQUES; i++) {
        cache->bloques[i].valido = false;
    }
}
