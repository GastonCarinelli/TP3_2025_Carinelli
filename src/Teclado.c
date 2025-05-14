/*
 * Teclado.c
 *
 *  Created on: 7 may. 2025
 *      Author: gasty
 */

#include "Teclado.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

typedef struct {
    GPIO_TypeDef* puerto;
    uint16_t pin;
} PinTeclado;

// Filas F1=PE8 F2=PE9 F3=PE10 F4=PE11
PinTeclado filas[4] = {
    { GPIOE, GPIO_Pin_8 },
    { GPIOE, GPIO_Pin_9 },
    { GPIOE, GPIO_Pin_10 },
    { GPIOE, GPIO_Pin_11 }
};

// Columnas C1=PC0 C2=PC3 C3=PC2 C4=PA0
PinTeclado columnas[4] = {
    { GPIOC, GPIO_Pin_0 },
    { GPIOC, GPIO_Pin_3 },
    { GPIOC, GPIO_Pin_2 },
    { GPIOA, GPIO_Pin_0 }
};

EstadoTeclado estado = ESPERANDO_TECLA;
static int fila_actual = 0;
static int columna_actual = 0;
static char tecla_actual = 0;

void inicializo_Teclado(void) {
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |RCC_AHB1Periph_GPIOC |RCC_AHB1Periph_GPIOE |RCC_AHB1Periph_GPIOD, ENABLE);

    // Filas como salidas
    for (int f = 0; f < 4; f++) {
        GPIO_InitStruct.GPIO_Pin = filas[f].pin;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(filas[f].puerto, &GPIO_InitStruct);
    }

    // Columnas como entradas
    for (int c = 0; c < 4; c++) {
        GPIO_InitStruct.GPIO_Pin = columnas[c].pin;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
        GPIO_Init(columnas[c].puerto, &GPIO_InitStruct);
    }
}

char Leer_Teclado(void) {
    char teclado[4][4] = {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}
    };

    switch (estado) {
        case ESPERANDO_TECLA:
            for (fila_actual = 0; fila_actual < 4; fila_actual++) {
                GPIO_SetBits(filas[fila_actual].puerto, filas[fila_actual].pin);
                for (columna_actual = 0; columna_actual < 4; columna_actual++) {
                    if (GPIO_ReadInputDataBit(columnas[columna_actual].puerto, columnas[columna_actual].pin) == 1) {
                        estado = DEBOUNCE;
                        break;
                    }
                }
                GPIO_ResetBits(filas[fila_actual].puerto, filas[fila_actual].pin);
                if (estado == DEBOUNCE) {
                    break;
                }
            }
            break;

        case DEBOUNCE:
            delay_ms(20);
            GPIO_SetBits(filas[fila_actual].puerto, filas[fila_actual].pin);
            estado = ESPERANDO_LIBERACION_TECLA;
            break;

        case ESPERANDO_LIBERACION_TECLA:
            if (GPIO_ReadInputDataBit(columnas[columna_actual].puerto, columnas[columna_actual].pin) == 0) {
                tecla_actual = 0;
                GPIO_ResetBits(filas[fila_actual].puerto, filas[fila_actual].pin);
                estado = ESPERANDO_TECLA;
            } else {
                tecla_actual = teclado[fila_actual][columna_actual];
                estado = ESPERANDO_LIBERACION_TECLA;
            }
            break;
    }

    return tecla_actual;
}


