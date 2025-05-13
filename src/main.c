/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#include "stm32f4xx.h"
#include "Teclado.h"

int tiempo = 100;

typedef enum {
    ESPERO_TECLA,
    BASES_TIEMPO,
    ESPERANDO_SUELTE_TECLA
} EstadoMain;

EstadoMain estado2;

void inicializo_LED(void);
void led_blink(char, int);
void delay_ms(int);

int main(void) {
    inicializo_Teclado();
    inicializo_LED();

    char tecla_anterior = 0;

    while (1) {
        switch (estado2) {
            case ESPERO_TECLA: {
                char tecla = Leer_Teclado();
                if (tecla != 0) {
                    estado2 = BASES_TIEMPO;
                }
                break;
            }

            case BASES_TIEMPO: {
                char tecla = Leer_Teclado();
                switch (tecla) {
                    case 'A':
                        tiempo = 50;
                        break;
                    case 'B':
                        tiempo = 90;
                        break;
                    case 'C':
                        tiempo = 110;
                        break;
                    case 'D':
                        tiempo = 220;
                        break;
                    case '*':
                        tiempo = 100;
                        break;
                    case '#':
                        tiempo = 100;
                        break;
                }

                if (tecla != 0) {
                    led_blink(tecla, tiempo);
                    tecla_anterior = tecla;
                    estado2 = ESPERANDO_SUELTE_TECLA;
                } else {
                    estado2 = ESPERO_TECLA;
                }
                break;
            }
            case ESPERANDO_SUELTE_TECLA: {
                if (Leer_Teclado() == 0) {
                    estado2 = ESPERO_TECLA;
                }
                break;
            }
        }
    }
}

void delay_ms(int tiempo) {
    int i;
    for (i = 0; i < tiempo * 12700; i++) {}
}

void inicializo_LED(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStruct);
}

void led_blink(char n, int tiempo) {
    if (n < 65) {
        for (char i = '0'; i < (n + 1); i++) {
            GPIO_SetBits(GPIOA, GPIO_Pin_5);
            delay_ms(tiempo);
            GPIO_ResetBits(GPIOA, GPIO_Pin_5);
            delay_ms(tiempo);
        }
    }
}
