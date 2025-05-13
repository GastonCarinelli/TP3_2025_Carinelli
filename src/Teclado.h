/*
 * Teclado.h
 *
 *  Created on: 7 may. 2025
 *      Author: gasty
 */

#ifndef TECLADO_H
#define TECLADO_H

#include "stm32f4xx.h"

typedef enum {
    ESPERANDO_TECLA,
    DEBOUNCE,
    ESPERANDO_LIBERACION_TECLA
} EstadoTeclado;


void inicializo_Teclado(void);
char Leer_Teclado(void);

#endif /* TECLADO_H */
