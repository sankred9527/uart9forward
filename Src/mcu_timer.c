#include "mcu_timer.h"


void timer_set(struct mcu_timer *t, uint32_t interval)
{
    t->start = clock_time();
    t->interval = interval;
}

bool timer_expired(struct mcu_timer *t)
{
    return (int)(clock_time() - t->start) >= (int)t->interval;
}


