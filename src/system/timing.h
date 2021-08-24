#ifndef TIMING_H
#define TIMING_H

#include <psp2/kernel/processmgr.h>
#include <sys/types.h>

typedef struct 
{
    timer_t time;
    SceUInt64 timeout;
    uint8_t elapsed;
} timing_timer;

/**
 * @brief gets the delta time in ms. (the duration since this function was last called)
 * 
 * @param sysclock the system kernel clock pointer to use
 * @return SceUInt64 the delta time in ms
 */
SceUInt64
timing_get_deltatime(SceKernelSysClock *sysclock);

/**
 * @brief calculates the fps based on the given delta time
 * 
 * @param dt the delta time in ms
 * @return uint8_t the fps
 */
uint8_t timing_get_fps(SceUInt64 dt);

/**
 * @brief updates a timer with the last frametime
 * 
 * @param timer the timer to update
 * @param dt the deltatime
 */
void timing_update_timer(timing_timer* timer, SceUInt64 dt);

/**
 * @brief checks if the given timer has elapsed or not
 * 
 * @param timer the timer to check
 * @return uint8_t 0 if false, nonzero if true
 */
uint8_t timing_check_timer_elapsed(timing_timer* timer);

#endif