#include "uart_utils.h"
#include "main.h"
#include <string.h>
#include <stdint.h>

void  Uart5SendString(char* str)
{
    HAL_GPIO_WritePin(RTS5S_GPIO_Port, RTS5S_Pin, GPIO_PIN_SET);
    HAL_UART_Transmit(&huart5, (uint8_t*)str,strlen(str), 50);
    HAL_GPIO_WritePin(RTS5S_GPIO_Port, RTS5S_Pin, GPIO_PIN_RESET);
}
