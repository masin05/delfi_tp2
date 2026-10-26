#include "memoria.h"

#include <string.h>

void memoria_inicializar(MemoriaPrincipal *memoria) {
    memset(memoria->datos, 0, TAM_MEMORIA);
}

bool memoria_rango_valido(size_t direccion, size_t n_bytes) {
    /* Se compara así (y no direccion + n_bytes <= TAM_MEMORIA) para que la suma no desborde. */
    return direccion <= TAM_MEMORIA && n_bytes <= TAM_MEMORIA - direccion;
}

bool memoria_leer(const MemoriaPrincipal *memoria, size_t direccion, size_t n_bytes, uint8_t *buffer) {
    if (!memoria_rango_valido(direccion, n_bytes)) {
        return false;
    }
    memcpy(buffer, memoria->datos + direccion, n_bytes);
    return true;
}

bool memoria_escribir(MemoriaPrincipal *memoria, size_t direccion, size_t n_bytes, const uint8_t *buffer) {
    if (!memoria_rango_valido(direccion, n_bytes)) {
        return false;
    }
    memcpy(memoria->datos + direccion, buffer, n_bytes);
    return true;
}

static bool palabra_libre(const MemoriaPrincipal *memoria, size_t indice_palabra) {
    for (size_t byte = 0; byte < TAM_PALABRA; byte++) {
        if (memoria->datos[indice_palabra * TAM_PALABRA + byte] != 0) {
            return false;
        }
    }
    return true;
}

void memoria_compactar(MemoriaPrincipal *memoria) {
    size_t destino = 0; /* próxima posición libre al comienzo de la memoria */

    for (size_t origen = 0; origen < PALABRAS_MEMORIA; origen++) {
        if (palabra_libre(memoria, origen)) {
            continue;
        }
        if (origen != destino) {
            memcpy(memoria->datos + destino * TAM_PALABRA, memoria->datos + origen * TAM_PALABRA, TAM_PALABRA);
            memset(memoria->datos + origen * TAM_PALABRA, 0, TAM_PALABRA);
        }
        destino++;
    }
}
