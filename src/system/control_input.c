#include "control_input.h"

void ctrl_input_get_leftstick(SceCtrlData *pad, stick_data *stickdata)
{
    int16_t lxpos = -128 + pad->lx; // full left is 0 so -128 and full right is 255 so 128
    int16_t lypos = -128 + pad->ly;

    stickdata->x = lxpos;
    stickdata->y = lypos;
}

void ctrl_input_get_rightstick(SceCtrlData *pad, stick_data *stickdata)
{
    int16_t rxpos = -128 + pad->rx; // full left is 0 so -128 and full right is 255 so 128
    int16_t rypos = -128 + pad->ry;

    stickdata->x = rxpos;
    stickdata->y = rypos;
}

float ctrl_input_calc_value(int16_t pos, SceUInt64 deltaTime)
{
    return (float)((pos) * (deltaTime / 1000.0)) * PLAYER_SPEED;
}