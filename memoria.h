#ifndef MEMORIA_H
#define MEMORIA_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define TAM_PALABRA 4          // bytes por palabra
#define PALABRAS_MEMORIA 1024  // capacidad de la memoria principal, en palabras
#define TAM_MEMORIA (PALABRAS_MEMORIA * TAM_PALABRA)

// la memoria principal es un arreglo de bytes, las direcciones son posiciones en ese arreglo
typedef struct {
    uint8_t datos[TAM_MEMORIA];
} MemoriaPrincipal;

void memoria_inicializar(MemoriaPrincipal *memoria);

// indica si [direccion, direccion + n_bytes) cabe en la memoria
bool memoria_rango_valido(size_t direccion, size_t n_bytes);

bool memoria_leer(const MemoriaPrincipal *memoria, size_t direccion, size_t n_bytes, uint8_t *buffer);
bool memoria_escribir(MemoriaPrincipal *memoria, size_t direccion, size_t n_bytes, const uint8_t *buffer);

// junta las palabras en uso al principio de la memoria, sin cambiar su orden, para eliminar los huecos. simplificación del modelo: una palabra con todos sus bytes en 0 cuenta como libre.
void memoria_compactar(MemoriaPrincipal *memoria);

#endif
