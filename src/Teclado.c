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

PinTeclado filas[4] = {
    { GPIOE, GPIO_Pin_11 },
    { GPIOE, GPIO_Pin_10 },
    { GPIOE, GPIO_Pin_9  },
    { GPIOE, GPIO_Pin_8  }
};

PinTeclado columnas[4] = {
    { GPIOA, GPIO_Pin_0 },
    { GPIOC, GPIO_Pin_2 },
    { GPIOC, GPIO_Pin_3 },
    { GPIOC, GPIO_Pin_0 }
};


EstadoTeclado estado = ESPERANDO_TECLA;
static int fila_actual = 0;
static int columna_actual = 0;
static char tecla_actual = 0;

void inicializo_Teclado(void) {
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOE, ENABLE);

    // Filas como salidas
    for (int f = 0; f < 4; f++) {
        GPIO_InitStruct.GPIO_Pin = filas[f].pin;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(filas[f].puerto, &GPIO_InitStruct);
    }

    // Columnas como entradas con PULL-DOWN
    for (int c = 0; c < 4; c++) {
        GPIO_InitStruct.GPIO_Pin = columnas[c].pin;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
        GPIO_Init(columnas[c].puerto, &GPIO_InitStruct);
    }
}

char Leer_Teclado(void) {
	static char teclado[4][4] = {
	    {'D', 'C', 'B', 'A'},
	    {'#', '9', '6', '3'},
	    {'0', '8', '5', '2'},
	    {'*', '7', '4', '1'}
	};

	switch (estado) {
	        case ESPERANDO_TECLA:
	            for (fila_actual = 0; fila_actual < 4; fila_actual++) {
	                GPIO_SetBits(filas[fila_actual].puerto, filas[fila_actual].pin);
	                for (columna_actual = 0; columna_actual < 4; columna_actual++) {
	                    if (GPIO_ReadInputDataBit(columnas[columna_actual].puerto, columnas[columna_actual].pin) == 1) {
	                        estado = DEBOUNCE;
	                        return 0;
	                    }
	                }
	                GPIO_ResetBits(filas[fila_actual].puerto, filas[fila_actual].pin);
	            }
	            break;

	        case DEBOUNCE:
	            delay_ms(20);
	            GPIO_SetBits(filas[fila_actual].puerto, filas[fila_actual].pin);
	            estado = VALIDAR_TECLA;
	            break;

	        case VALIDAR_TECLA:
	            if (GPIO_ReadInputDataBit(columnas[columna_actual].puerto, columnas[columna_actual].pin) == 1) {
	                tecla_actual = teclado[fila_actual][columna_actual];
	                estado = TECLA_VALIDA;
	            } else {
	                estado = ESPERANDO_TECLA;
	            }
	            GPIO_ResetBits(filas[fila_actual].puerto, filas[fila_actual].pin);
	            break;

	        case TECLA_VALIDA:
	            estado = ESPERANDO_LIBERACION_TECLA;
	            return tecla_actual;

	        case ESPERANDO_LIBERACION_TECLA: {
	            int tecla_presionada = 0;

	            // Escaneo completo: filas y columnas
	            for (int f = 0; f < 4; f++) {
	                GPIO_SetBits(filas[f].puerto, filas[f].pin);
	                for (int c = 0; c < 4; c++) {
	                    if (GPIO_ReadInputDataBit(columnas[c].puerto, columnas[c].pin) == 1) {
	                        tecla_presionada = 1;
	                        break;
	                    }
	                }
	                GPIO_ResetBits(filas[f].puerto, filas[f].pin);
	            }

	            if (!tecla_presionada) {
	                estado = ESPERANDO_TECLA;
	            }

	            return 0;
	        }
	    }

	    return 0;
	}
