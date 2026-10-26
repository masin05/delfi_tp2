#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memoria.h"

static void leer_palabra(Memoria *memoria, size_t indice_palabra) {
    uint8_t palabra[TAM_PALABRA];
    memoria_leer(memoria, indice_palabra * TAM_PALABRA, TAM_PALABRA, palabra);
}

// alta localidad temporal: se accede una y otra vez a las mismas 8 palabras (cada una en un bloque distinto)
static void programa_alta_localidad_temporal(Memoria *memoria) {
    for (int repeticion = 0; repeticion < 100; repeticion++) {
        for (size_t i = 0; i < 8; i++) {
            leer_palabra(memoria, i * PALABRAS_POR_BLOQUE);
        }
    }
}

// alta localidad espacial: se recorre toda la memoria en orden, una sola vez
static void programa_alta_localidad_espacial(Memoria *memoria) {
    for (size_t palabra = 0; palabra < PALABRAS_MEMORIA; palabra++) {
        leer_palabra(memoria, palabra);
    }
}

// baja localidad: accesos a palabras al azar de toda la memoria (mucho más grande que la caché)
static void programa_baja_localidad(Memoria *memoria) {
    srand(1); // semilla fija para que los resultados sean siempre los mismos
    for (int i = 0; i < 1000; i++) {
        leer_palabra(memoria, (size_t)rand() % PALABRAS_MEMORIA);
    }
}

static void ejecutar_programa(const char *nombre, void (*programa)(Memoria *)) {
    Memoria memoria;
    memoria_inicializar(&memoria);
    programa(&memoria);

    Estadisticas estadisticas = memoria_obtener_estadisticas(&memoria);
    printf("%-26s hits: %4zu  misses: %4zu  tasa de efectividad: %6.2f%%\n", nombre, estadisticas.hits,
           estadisticas.misses, 100.0 * estadisticas.tasa_efectividad);
}

// escribe 3 palabras separadas por huecos (una es un dato 0), compacta y comprueba que se conservan
static void probar_escritura_y_compactacion(void) {
    Memoria memoria;
    memoria_inicializar(&memoria);

    const size_t palabra_de[3] = {0, 3, 9};
    const uint8_t valor_de[3] = {0xA1, 0x00, 0xA3};
    uint8_t palabra[TAM_PALABRA];

    for (int i = 0; i < 3; i++) {
        memset(palabra, valor_de[i], TAM_PALABRA);
        memoria_escribir(&memoria, palabra_de[i] * TAM_PALABRA, TAM_PALABRA, palabra);
    }
    leer_palabra(&memoria, 9); // deja la palabra 9 en la caché, tras compactar no debe devolver el dato viejo

    memoria_compactar(&memoria);

    bool correcto = true;
    for (size_t i = 0; i < 3; i++) { // ahora deben estar en las palabras 0, 1 y 2, incluso el dato 0
        memoria_leer(&memoria, i * TAM_PALABRA, TAM_PALABRA, palabra);
        correcto = correcto && palabra[0] == valor_de[i];
    }
    memoria_leer(&memoria, 9 * TAM_PALABRA, TAM_PALABRA, palabra);
    correcto = correcto && palabra[0] == 0; // el lugar viejo quedó libre

    printf("Escritura y compactacion: %s\n", correcto ? "OK" : "FALLO");
}

int main(void) {
    printf("Memoria: %d palabras de %d bytes | bloque: %d palabras | cache: %d bloques (FIFO, write-through)\n\n",
           PALABRAS_MEMORIA, TAM_PALABRA, PALABRAS_POR_BLOQUE, NUM_BLOQUES);

    ejecutar_programa("Alta localidad temporal", programa_alta_localidad_temporal);
    ejecutar_programa("Alta localidad espacial", programa_alta_localidad_espacial);
    ejecutar_programa("Baja localidad", programa_baja_localidad);
    printf("\n");
    probar_escritura_y_compactacion();
    return EXIT_SUCCESS;
}
