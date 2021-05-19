#ifndef TIMING_H
#define TIMING_H

#include <psp2/kernel/processmgr.h>

/**
 * @brief gets the delta time in ms. (the duration since this function was last called)
 * 
 * @param sysclock the system kernel clock pointer to use
 * @return SceUInt64 the delta time in ms
 */
SceUInt64 timing_get_deltatime(SceKernelSysClock *sysclock);

/**
 * @brief calculates the fps based on the given delta time
 * 
 * @param dt the delta time in ms
 * @return uint8_t the fps
 */
uint8_t timing_get_fps(SceUInt64 dt);
#endif