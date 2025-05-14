/*
 * Teclado.c
 *
 *  Created on: 7 may. 2025
 *      Author: gasty
 */

#include "Teclado.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

//Filas F1=PE8 F2=PE9 F3=PE10 F4=PE11
uint32_t fil_puertos[4] = { GPIOE_BASE, GPIOE_BASE, GPIOE_BASE, GPIOE_BASE };
uint16_t fil_pins[4] = { GPIO_Pin_8, GPIO_Pin_9, GPIO_Pin_10, GPIO_Pin_11 };

//Columnas C1=PC0 C2=PC3 C3=PC2 C4=PA0
uint32_t col_puertos[4] = { GPIOC_BASE, GPIOC_BASE, GPIOC_BASE, GPIOA_BASE };
uint16_t col_pins[4] = { GPIO_Pin_0, GPIO_Pin_3, GPIO_Pin_2, GPIO_Pin_0 };

EstadoTeclado estado = ESPERANDO_TECLA;
static int fila_actual = 0;
static int columna_actual = 0;
static char tecla_actual = 0;

void inicializo_Teclado(void) {
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOE| RCC_AHB1Periph_GPIOD, ENABLE);

    int f,c;

    // Filas como salidas
    for (f = 0; f < 4; f++) {
        GPIO_InitStruct.GPIO_Pin = fil_pins[f];
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init((GPIO_TypeDef *)fil_puertos[f], &GPIO_InitStruct);
    }

    // Columnas como entradas
    for (c = 0; c < 4; c++) {
        GPIO_InitStruct.GPIO_Pin = col_pins[c];
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
        GPIO_Init((GPIO_TypeDef *)col_puertos[c], &GPIO_InitStruct);
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
                GPIO_SetBits((GPIO_TypeDef *)fil_puertos[fila_actual], fil_pins[fila_actual]);
                for (columna_actual = 0; columna_actual < 4; columna_actual++) {
                    if (GPIO_ReadInputDataBit((GPIO_TypeDef *)col_puertos[columna_actual], col_pins[columna_actual]) == 1) {
                        estado = DEBOUNCE;
                        break;
                    }
                }
                GPIO_ResetBits((GPIO_TypeDef *)fil_puertos[fila_actual], fil_pins[fila_actual]);
                if (estado == DEBOUNCE) {
                    break;
                }
            }
            break;

        case DEBOUNCE:
            delay_ms(20);
            GPIO_SetBits((GPIO_TypeDef *)fil_puertos[fila_actual], fil_pins[fila_actual]);
            estado = ESPERANDO_LIBERACION_TECLA;

            break;

        case ESPERANDO_LIBERACION_TECLA:
            if (GPIO_ReadInputDataBit((GPIO_TypeDef *)col_puertos[columna_actual], col_pins[columna_actual]) == 0) {
                tecla_actual=0;
                GPIO_ResetBits((GPIO_TypeDef *)fil_puertos[fila_actual], fil_pins[fila_actual]);
            	estado = ESPERANDO_TECLA;
            }
            else
            {
            	tecla_actual=teclado[fila_actual][columna_actual];
            	estado= ESPERANDO_LIBERACION_TECLA;
            }
            break;
    }
    return tecla_actual;
}

