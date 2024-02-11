
#ifndef MCU_TIMER_H
#define MCU_TIMER_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f1xx_hal.h"

struct mcu_timer
{
    uint32_t start, interval;
};

#define clock_time HAL_GetTick

// inline uint32_t clock_time(void)
// {
//     return HAL_GetTick();
// }

void timer_set(struct mcu_timer *t, uint32_t interval);
bool timer_expired(struct mcu_timer *t);

#endif