#include "uart_utils.h"
#include "main.h"
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
    UART_HandleTypeDef *uart_instance;
    volatile bool tx_completed; 
    volatile bool rx_completed; 

    /*
     usart1-3 通过端口复用来配置, 需要定义2组不同的gpio
    */
    gpio_combine_t tx_gpio[2]; 
    gpio_combine_t rx_gpio[2];

    gpio_combine_t rts_gpio[2];

    //仅仅 huart4 才需要这个, 因为 UART4没有复用端口, 通过2个 RTS GPIO端口去切
    gpio_combine_t rts_extra_gpio[2];    
} uart_contex_t;

static uart_contex_t gl_all_uarts[5];
#define MAX_UART_CHANNEL (2)
static int gl_current_channel = 0;


#define HANDLE_UART5 (gl_all_uarts + 4)

// void Uart5SendString(char* str)
// {
//     HAL_GPIO_WritePin(RTS5S_GPIO_Port, RTS5S_Pin, GPIO_PIN_SET);
//     HAL_UART_Transmit(&huart5, (uint8_t*)str,strlen(str), 50);
//     HAL_GPIO_WritePin(RTS5S_GPIO_Port, RTS5S_Pin, GPIO_PIN_RESET);
// }

// void Uart5RecvString(char* str)
// {
//     char data[128];    
//     HAL_UART_Receive(&huart5, data, 128, 100);    
// }

void uart_app_init(void)
{
    memset(gl_all_uarts, 0, sizeof(gl_all_uarts));
    
    gl_all_uarts[0] = (uart_contex_t) {
        .uart_instance = &huart1,
        .tx_gpio = {  { .gpiox = TXD1_0_GPIO_Port, .gpio_pin = TXD1_0_Pin } ,  { .gpiox = TXD1_1_GPIO_Port, .gpio_pin = TXD1_1_Pin } },
        .rx_gpio = {  { .gpiox = RXD1_0_GPIO_Port, .gpio_pin = RXD1_0_Pin } ,  { .gpiox = RXD1_1_GPIO_Port, .gpio_pin = RXD1_1_Pin } },
        .rts_gpio = { { .gpiox = RTS1_0_GPIO_Port, .gpio_pin = RTS1_0_Pin } ,  { .gpiox = RTS1_1S_GPIO_Port, .gpio_pin = RTS1_1S_Pin } },
    };

    gl_all_uarts[1] = (uart_contex_t) {
        .uart_instance = &huart2,
        .tx_gpio = {  { .gpiox = TXD2_0_GPIO_Port, .gpio_pin = TXD2_0_Pin } ,  { .gpiox = TXD2_1_GPIO_Port, .gpio_pin = TXD2_1_Pin } },
        .rx_gpio = {  { .gpiox = RXD2_0_GPIO_Port, .gpio_pin = RXD2_0_Pin } ,  { .gpiox = RXD2_1_GPIO_Port, .gpio_pin = RXD2_1_Pin } },
        .rts_gpio = { { .gpiox = RTS2_0_GPIO_Port, .gpio_pin = RTS2_0_Pin } ,  { .gpiox = RTS2_1_GPIO_Port, .gpio_pin = RTS2_1_Pin } },
    };

    gl_all_uarts[2] = (uart_contex_t) {
        .uart_instance = &huart3,
        .tx_gpio = {  { .gpiox = TXD3_0_GPIO_Port, .gpio_pin = TXD3_0_Pin } ,  { .gpiox = TXD3_1_GPIO_Port, .gpio_pin = TXD3_1_Pin } },
        .rx_gpio = {  { .gpiox = RXD3_0_GPIO_Port, .gpio_pin = RXD3_0_Pin } ,  { .gpiox = RXD3_1_GPIO_Port, .gpio_pin = RXD3_1_Pin } },
        .rts_gpio = { { .gpiox = RTS3_0_GPIO_Port, .gpio_pin = RTS3_0_Pin } ,  { .gpiox = RTS3_1_GPIO_Port, .gpio_pin = RTS3_1_Pin } },
    };
        
    gl_all_uarts[3] = (uart_contex_t) {
        .uart_instance = &huart4,        
        .rts_gpio = { { .gpiox = RTS4_0S_GPIO_Port, .gpio_pin = RTS4_0S_Pin } ,  { .gpiox = RTS4_1S_GPIO_Port, .gpio_pin = RTS4_1S_Pin } },
    };

    gl_all_uarts[4] = (uart_contex_t) {
        .uart_instance = &huart5,       
        .rts_gpio = { { .gpiox = RTS5S_GPIO_Port, .gpio_pin = RTS5S_Pin } },
    };
    
}

static void uart_switch_channel(int channel)
{
    //TODO: __HAL_AFIO_REMAP_USART3_ENABLE 
    if ( channel != gl_current_channel) 
    {
        if ( channel == 0 )
        {
            __HAL_AFIO_REMAP_USART1_DISABLE();
            __HAL_AFIO_REMAP_USART2_DISABLE();
            __HAL_AFIO_REMAP_USART3_DISABLE();
            //TODO: switch uart4       
        } else {
            __HAL_AFIO_REMAP_USART1_ENABLE();
            __HAL_AFIO_REMAP_USART2_ENABLE();
            __HAL_AFIO_REMAP_USART3_ENABLE();
            //TODO: switch uart4
        }
        gl_current_channel = channel;
    }    
}

static void uart_up_send(uint8_t *data, uint16_t size)
{
    HANDLE_UART5->tx_completed = false;
    HAL_GPIO_WritePin(HANDLE_UART5->rts_gpio[0].gpiox, HANDLE_UART5->rts_gpio[0].gpio_pin, GPIO_PIN_SET);
    HAL_UART_Transmit_IT(HANDLE_UART5->uart_instance, data, size); 
}

static bool uart_up_send_done(void)
{
    return HANDLE_UART5->tx_completed;
}

static void uart_up_recv(uint8_t *data, uint16_t size)
{    
    HANDLE_UART5->rx_completed = false;
    HAL_UARTEx_ReceiveToIdle_IT(HANDLE_UART5->uart_instance, data, size);
}

// void uart_up_print(const char *format, ...) {
//     #define MAX_BUFFER_SIZE 128u
//     static char buffer[MAX_BUFFER_SIZE];
    
//     va_list args;
//     va_start(args, format);
//     int n = vsnprintf(buffer, MAX_BUFFER_SIZE, format, args);
//     va_end(args);
    
//     HAL_UART_Transmit_IT(&huart5, (uint8_t*)buffer, n);
//     //printf("Printed to static array: %s\n", buffer);
// }


/// @brief HAL callback for tx complete in IRQ handler
/// @param huart /
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if ( huart->Instance == USART1 || huart->Instance == USART2 || huart->Instance == USART3 || huart->Instance == UART4 )
    {
    }
    else if ( huart->Instance == UART5 )
    {        
        HAL_GPIO_WritePin(HANDLE_UART5->rts_gpio[0].gpiox, HANDLE_UART5->rts_gpio[0].gpio_pin, GPIO_PIN_RESET);
        HANDLE_UART5->tx_completed = true;
    }
    
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{

}

void uart_thread(void)
{
    while(1)
    {

    }
}

void uart_test(void)
{
    
}