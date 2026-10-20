#include "memoria.h"

MemoriaPrincipal* crearMemoria(size_t capacidad){
  MemoriaPrincipal *mem = (MemoriaPrincipal*) malloc(sizeof(MemoriaPrincipal));
  if (!mem) return NULL;
  mem->datos = (uint8_t*) calloc(capacidad, sizeof(uint8_t));
  mem->capacidad = capacidad;
  if(!mem -> datos){
    destruirMemoria(mem);
  }
  return mem;
}

void destruirMemoria(MemoriaPrincipal mem){
  free(mem->datos);
  free(mem);
}

int leerMemoria (MemoriaPrincipal *memoria, size_t direccion, size_t n_bytes, unit8_t *buffer){
  
}
  
