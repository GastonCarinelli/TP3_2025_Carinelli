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
    PROCESO_TECLA,
    ESPERANDO_LIBERACION_MAIN
} EstadoMain;

EstadoMain estado2;

void inicializo_LED(void);
void led_blink(char cantidad, int tiempo);
void delay_ms(int ms);

int main(void) {
    inicializo_Teclado();
    inicializo_LED();

    char tecla_anterior = 0;

    while (1) {
        switch (estado2) {

            case ESPERO_TECLA: {
                char tecla = Leer_Teclado();
                if (tecla != 0) {
                    tecla_anterior = tecla;
                    estado2 = PROCESO_TECLA;
                }
                break;
            }

            case PROCESO_TECLA: {
                const int tiempos[] = {50, 90, 110, 220};

                if (tecla_anterior >= 'A' && tecla_anterior <= 'D') {
                    tiempo = tiempos[tecla_anterior - 'A'];
                }
                else if (tecla_anterior >= '0' && tecla_anterior <= '9') {
                    int cantidad = (tecla_anterior - '0') + 1;
                    led_blink(cantidad, tiempo);
                }

                estado2 = ESPERANDO_LIBERACION_MAIN;
                break;
            }

            case ESPERANDO_LIBERACION_MAIN: {

                if (Leer_Teclado() == 0) {
                    estado2 = ESPERO_TECLA;
                }
                break;
            }
        }
    }
}

void delay_ms(int ms) {
    for (int i = 0; i < ms * 12700; i++) {}
}

// Inicializa LED en PD10
void inicializo_LED(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStruct);
}

void led_blink(char cantidad, int tiempo) {
    for (char i = 0; i < cantidad; i++) {
        GPIO_SetBits(GPIOD, GPIO_Pin_10);
        delay_ms(tiempo);
        GPIO_ResetBits(GPIOD, GPIO_Pin_10);
        delay_ms(tiempo);
    }
}
