#include "drawing.h"

void drawing_draw_rectangle_open(float x, float y, float width, float heigth, int thiccness, unsigned int color)
{
    for (int i = 0; i < thiccness; i++)
    {
        vita2d_draw_line(x, y + i, x + width, y + i, color);                   // top
        vita2d_draw_line(x + i, y, x + i, y + heigth, color);                  // left
        vita2d_draw_line(x, y - i + heigth, x + width, y - i + heigth, color); // bottom
        vita2d_draw_line(x + width - i, y, x + width - i, y + heigth, color);  // right
    }
}

void drawing_draw_hline(float x0, float y0, float width, int thiccness, unsigned int color)
{
    for (int i = 0; i < thiccness; i++)
    {
        vita2d_draw_line(x0, y0 + i, x0 + width, y0 + i, color);
    }
}

void drawing_draw_vline(float x0, float y0, float height, int thiccness, unsigned int color)
{
    for (int i = 0; i < thiccness; i++)
    {
        vita2d_draw_line(x0 + i, y0, x0 + i, y0 + height, color);
    }
}