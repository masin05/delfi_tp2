#include "cache.h"

#include <string.h>

static BloqueCache *buscar_bloque(Cache *cache, size_t numero_bloque) {
    for (size_t i = 0; i < NUM_BLOQUES; i++) {
        BloqueCache *bloque = &cache->bloques[i];
        if (bloque->valido && bloque->numero_bloque == numero_bloque) {
            return bloque;
        }
    }
    return NULL;
}

void cache_inicializar(Cache *cache) {
    memset(cache, 0, sizeof *cache);
}

bool cache_leer(Cache *cache, size_t indice_palabra, uint8_t palabra[TAM_PALABRA]) {
    BloqueCache *bloque = buscar_bloque(cache, indice_palabra / PALABRAS_POR_BLOQUE);
    if (bloque == NULL) {
        return false;
    }
    size_t inicio_en_bloque = (indice_palabra % PALABRAS_POR_BLOQUE) * TAM_PALABRA;
    memcpy(palabra, bloque->datos + inicio_en_bloque, TAM_PALABRA);
    return true;
}

void cache_escribir(Cache *cache, size_t numero_bloque, const uint8_t datos_bloque[TAM_BLOQUE]) {
    BloqueCache *bloque = buscar_bloque(cache, numero_bloque);

    if (bloque == NULL) {
        // los lugares se ocupan en orden y después se reemplazan en el mismo orden
        // así que el lugar apuntado siempre es el vacío o el que entró primero (FIFO)
        bloque = &cache->bloques[cache->proximo_a_reemplazar];
        cache->proximo_a_reemplazar = (cache->proximo_a_reemplazar + 1) % NUM_BLOQUES;
        bloque->valido = true;
        bloque->numero_bloque = numero_bloque;
    }
    memcpy(bloque->datos, datos_bloque, TAM_BLOQUE);
}

void cache_invalidar(Cache *cache) {
    for (size_t i = 0; i < NUM_BLOQUES; i++) {
        cache->bloques[i].valido = false;
    }
    cache->proximo_a_reemplazar = 0;
}
