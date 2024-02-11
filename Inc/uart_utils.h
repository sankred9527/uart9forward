#ifndef UART_UTILS_H
#define UART_UTILS_H


#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"

typedef struct  {
    GPIO_TypeDef *gpiox;
    uint16_t gpio_pin;
} gpio_combine_t;

typedef struct __uart_contex {
    UART_HandleTypeDef uart_instance;
    volatile bool tx_completed; 
    volatile bool rx_completed; 
    IRQn_Type irqt;
    int current_channel;
    int index;

    /*
     usart1-3 通过端口复用来配置, 需要定义2组不同的gpio
    */
    gpio_combine_t tx_gpio[2]; 
    gpio_combine_t rx_gpio[2];

    gpio_combine_t rts_gpio[2];
} uart_contex_t;



extern uart_contex_t gl_all_uarts[5];
#define UART_GET_HANDLE(n) (gl_all_uarts[n].uart_instance)

char uart_test1_thread(void);
void uart_thread(void);

#endif
