#include <psp2/kernel/processmgr.h>

#include "timing.h"
static SceUInt64 lastFrameTime = 0;

SceUInt64 timing_get_deltatime(SceKernelSysClock *sysclock)
{
    lastFrameTime = *sysclock; // put the last measured process time in lastFrameTime
    uint8_t ret = sceKernelGetProcessTime(sysclock); // sysclock now holds the current process time
    if (ret < 0)
        return -1;

    return (*sysclock - lastFrameTime) / 1000.0; // delta is the new time - the last time
}

uint8_t timing_get_fps(SceUInt64 dt)
{
    return (1 / (dt / 1000.0));
}
