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


void drawing_draw_window(float x, float y, float width, float heigth, const char *title_text, vita2d_pgf *pgf, unsigned int border_color)
{
    drawing_draw_rectangle_open(x, y, width, heigth, 10, border_color);
    drawing_draw_hline(x, y + 10, width, 20, border_color);
    for (int i = 0; i < 3; i++)
    {
        int box_x = width - 22 * i - 30 + x;
        vita2d_draw_rectangle(box_x, y + 6, 20, 20, COLOR_MAGENTA);
        drawing_draw_rectangle_open(box_x, y + 6, 20, 20, 2, COLOR_BLACK);
        switch (i)
        {
        case 0:
            for (int i = 0; i < 3; i++)
            {
                vita2d_draw_line(box_x + 4, 8 + i + y, box_x + 16, 20 + i + y, COLOR_BLACK);
                vita2d_draw_line(box_x + 16, 8 + i + y, box_x + 4, 20 + i + y, COLOR_BLACK);
            }
            break;
        case 1:
            drawing_draw_rectangle_open(box_x + 4, y + 12, 12, 10, 3, COLOR_BLACK);
            break;
        case 2:
            drawing_draw_hline(box_x + 4, y + 20, 12, 3, COLOR_BLACK);
            break;
        }
    }
    drawing_draw_rectangle_open(x, y, width, heigth, 2, COLOR_BLACK); // line around screen
    vita2d_pgf_draw_text(pgf, x + 10, y + 23, COLOR_MAGENTA, 0.9, title_text);
}

void drawing_draw_window_clear(float x, float y, float width, float heigth, const char *title_text, vita2d_pgf *pgf)
{
    drawing_draw_window(x,y,width,heigth,title_text,pgf,MAIN_BORDER_COLOR);
}

void drawing_draw_window_selected(float x, float y, float width, float heigth, const char *title_text, vita2d_pgf *pgf)
{
    drawing_draw_window(x,y,width,heigth,title_text,pgf,SELECTED_BORDER_COLOR);
}

void drawing_draw_window_filled(float x, float y, float width, float heigth, const char *title_text, vita2d_pgf *pgf, unsigned int color)
{
    drawing_draw_window_clear(x, y, width, heigth, title_text, pgf);
    vita2d_draw_rectangle(x + 10, y + 30, width - 20, heigth - 40, color);
}

void drawing_draw_selected_window_filled(float x, float y, float width, float heigth, const char *title_text, vita2d_pgf *pgf, unsigned int color)
{
    drawing_draw_window_selected(x, y, width, heigth, title_text, pgf);
    vita2d_draw_rectangle(x + 10, y + 30, width - 20, heigth - 40, color);
}

void drawing_draw_triangle(float x0, float y0, float x1, float y1, float x2, float y2, int thiccness, unsigned int color)
{
    for (int i = 0; i < thiccness; i++)
    {
        vita2d_draw_line(x0, y0 + i, x1, y1 + i, color);
        vita2d_draw_line(x1, y1 + i, x2, y2 + i, color);
        vita2d_draw_line(x2, y2 + i, x0, y0 + i, color);
    }
}

void drawing_draw_triangle_filled(float x0, float y0, float x1, float y1, float x2, float y2, unsigned int color)
{
    vita2d_draw_line(x0, y0, x1, y1, color);
    vita2d_draw_line(x1, y1, x2, y2, color);
    vita2d_draw_line(x2, y2, x0, y0, color);

    float x_start = x0;
    float y_start = y0;
    float x_end = x1;
    float y_end = y1;
    SceBool start_done = SCE_FALSE;
    SceBool end_done = SCE_FALSE;

    //something is not right with y_start
    while (start_done == SCE_FALSE && end_done == SCE_FALSE)
    {
        if (x_start < x2)
            x_start += 1.0;
        if (y_start < y2)
            y_start += 1.0;
        if (x_end < x2)
            x_end += 1.0;
        if (y_end < y2)
            y_end += 1.0;
        if (x_start >= x2 && y_start >= y2)
            start_done = SCE_TRUE;
        if (x_end >= x2 && y_end >= x2)
            end_done = SCE_TRUE;
        vita2d_draw_line(x_start, y_start, x_end, y_end, color);
    }
}
