#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>

#include <vita2d.h>

#include "sprites.h"

#define printf psvDebugScreenPrintf

void sprites_draw_bullet(BULLET *bullet)
{
    if (bullet->active)
    {
        vita2d_draw_rectangle(bullet->x, bullet->y, (float)5, (float)10, bullet->color);        
    }
}