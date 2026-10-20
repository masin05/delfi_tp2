#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint8_t *dato;
  size_t cantidad;
} MemoriaPrincipal;

MemoriaPrincipal* crearMemoria (size_t capacidad);
int leerMemoria (MemoriaPrincipal *memoria, size_t direccion, size_t n_bytes, unit8_t *buffer);
void destruirMemoria (MemoriaPrincipal *mem);
MemoriaPrincipal escribirMemoria (MemoriaPrincipal *mem, size_t direccion, size_t n_bytes, unit8_t *buffer);
void compactarMemoria (MemoriaPrincipal *mem)
