#include "drawing.h"

void drawing_draw_rectangle_open(float x, float y, float width, float heigth, int thiccness, unsigned int color)
{
    for (int i = 0; i < thiccness; i++)
    {
        vita2d_draw_line(x, y + i, x + width, y + i, color);
        vita2d_draw_line(x + i, y, x + i, y + heigth, color);
        vita2d_draw_line(x, y - i, x + width, y - i, color);
        vita2d_draw_line(x + width - i, y, x + width - i, y + heigth, color);
    }
}