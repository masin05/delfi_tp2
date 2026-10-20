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
  if (direccion + n_bytes > mem->capacidad) return -1;
  memcpy(buffer, mem->datos + direccion, n_bytes);
  return 0;
}

MemoriaPrincipal escribirMemoria (MemoriaPrincipal *mem, size_t direccion, size_t n_bytes, unit8_t *buffer){

}
  
MemoriaPrincipal compactarMemoria (MemoriaPrincipal *memoria, size_t direccion, size_t n_bytes, unit8_t *buffer){

}
