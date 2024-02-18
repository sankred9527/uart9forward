#ifndef GPIO_UTILS_H
#define GPIO_UTILS_H

#include <stdint.h>

#define LED_POWER_TYPE (0)
#define LED_RED_TYPE (1)
#define LED_GREEN_TYPE (2)

void SetLed(uint8_t ledtype, uint8_t ledidx, uint8_t status);
#endif

