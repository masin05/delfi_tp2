#include "memoria.h"

#include <string.h>

_Static_assert(TAM_MEMORIA % TAM_BLOQUE == 0, "la memoria debe contener una cantidad exacta de bloques");

static bool rango_valido(size_t direccion, size_t n_bytes) {
    // se compara así (y no direccion + n_bytes <= TAM_MEMORIA) para que la suma no desborde
    return direccion <= TAM_MEMORIA && n_bytes <= TAM_MEMORIA - direccion;
}

static uint8_t *palabra_en_memoria(Memoria *memoria, size_t indice_palabra) {
    return &memoria->datos[indice_palabra * TAM_PALABRA];
}

void memoria_inicializar(Memoria *memoria) {
    memset(memoria, 0, sizeof *memoria);
    cache_inicializar(&memoria->cache);
}

// copia el bloque desde la memoria a la caché (si ya estaba, lo actualiza)
static void copiar_bloque_a_cache(Memoria *memoria, size_t numero_bloque) {
    cache_escribir(&memoria->cache, numero_bloque, &memoria->datos[numero_bloque * TAM_BLOQUE]);
}

// lee una palabra completa, es un acceso: hit si estaba en la caché, miss si hubo que traer su bloque
static void leer_palabra(Memoria *memoria, size_t indice_palabra, uint8_t palabra[TAM_PALABRA]) {
    if (cache_leer(&memoria->cache, indice_palabra, palabra)) {
        memoria->hits++;
        return;
    }
    memoria->misses++;
    copiar_bloque_a_cache(memoria, indice_palabra / PALABRAS_POR_BLOQUE);
    memcpy(palabra, palabra_en_memoria(memoria, indice_palabra), TAM_PALABRA);
}

// escribe una palabra completa en la memoria y deja su bloque actualizado en la caché (write-through)
static void escribir_palabra(Memoria *memoria, size_t indice_palabra, const uint8_t palabra[TAM_PALABRA]) {
    memcpy(palabra_en_memoria(memoria, indice_palabra), palabra, TAM_PALABRA);
    memoria->palabra_en_uso[indice_palabra] = true;
    copiar_bloque_a_cache(memoria, indice_palabra / PALABRAS_POR_BLOQUE);
}

// un acceso de n_bytes desde `direccion` toca varias palabras
// para una de ellas calcula qué bytes del acceso le corresponden: dónde empiezan dentro de la palabra,
// dónde dentro del buffer y cuántos son
typedef struct {
    size_t inicio_en_palabra;
    size_t inicio_en_buffer;
    size_t cantidad;
} Tramo;

static Tramo tramo_en_palabra(size_t direccion, size_t n_bytes, size_t indice_palabra) {
    size_t inicio_palabra = indice_palabra * TAM_PALABRA;
    size_t desde = (direccion > inicio_palabra) ? direccion : inicio_palabra;
    size_t hasta = (direccion + n_bytes < inicio_palabra + TAM_PALABRA) ? direccion + n_bytes
                                                                         : inicio_palabra + TAM_PALABRA;
    return (Tramo){desde - inicio_palabra, desde - direccion, hasta - desde};
}

bool memoria_leer(Memoria *memoria, size_t direccion, size_t n_bytes, uint8_t *buffer) {
    if (!rango_valido(direccion, n_bytes)) {
        return false;
    }
    if (n_bytes == 0) {
        return true;
    }

    size_t primera_palabra = direccion / TAM_PALABRA;
    size_t ultima_palabra = (direccion + n_bytes - 1) / TAM_PALABRA;

    for (size_t indice = primera_palabra; indice <= ultima_palabra; indice++) {
        uint8_t palabra[TAM_PALABRA];
        leer_palabra(memoria, indice, palabra);

        Tramo tramo = tramo_en_palabra(direccion, n_bytes, indice);
        memcpy(buffer + tramo.inicio_en_buffer, palabra + tramo.inicio_en_palabra, tramo.cantidad);
    }
    return true;
}

bool memoria_escribir(Memoria *memoria, size_t direccion, size_t n_bytes, const uint8_t *datos) {
    if (!rango_valido(direccion, n_bytes)) {
        return false;
    }
    if (n_bytes == 0) {
        return true;
    }

    size_t primera_palabra = direccion / TAM_PALABRA;
    size_t ultima_palabra = (direccion + n_bytes - 1) / TAM_PALABRA;

    for (size_t indice = primera_palabra; indice <= ultima_palabra; indice++) {
        // se lee la palabra entera para conservar los bytes que esta escritura no modifica
        uint8_t palabra[TAM_PALABRA];
        leer_palabra(memoria, indice, palabra);

        Tramo tramo = tramo_en_palabra(direccion, n_bytes, indice);
        memcpy(palabra + tramo.inicio_en_palabra, datos + tramo.inicio_en_buffer, tramo.cantidad);
        escribir_palabra(memoria, indice, palabra);
    }
    return true;
}

void memoria_compactar(Memoria *memoria) {
    size_t destino = 0; // próxima posición libre al comienzo de la memoria

    for (size_t origen = 0; origen < PALABRAS_MEMORIA; origen++) {
        if (!memoria->palabra_en_uso[origen]) {
            continue;
        }
        if (origen != destino) {
            memcpy(palabra_en_memoria(memoria, destino), palabra_en_memoria(memoria, origen), TAM_PALABRA);
            memset(palabra_en_memoria(memoria, origen), 0, TAM_PALABRA);
            memoria->palabra_en_uso[destino] = true;
            memoria->palabra_en_uso[origen] = false;
        }
        destino++;
    }
    // los datos cambiaron de lugar, lo que guarda la caché ya no corresponde a esas direcciones
    cache_invalidar(&memoria->cache);
}

Estadisticas memoria_obtener_estadisticas(const Memoria *memoria) {
    size_t total = memoria->hits + memoria->misses;
    double tasa = (total == 0) ? 0.0 : (double)memoria->hits / (double)total;
    return (Estadisticas){memoria->hits, memoria->misses, tasa};
}
