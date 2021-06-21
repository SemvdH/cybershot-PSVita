#include "sprites.h"

#define printf psvDebugScreenPrintf

void sprites_draw_bullet(BULLET *bullet)
{
    if (bullet->active)
    {
        vita2d_draw_rectangle(bullet->x, bullet->y, (float)3, (float)8, bullet->color);
    }
}

void sprites_draw_smoke_circle(SMOKE_PARTICLE *smoke_particle)
{
    if (smoke_particle->active)
    {
        vita2d_draw_fill_circle(smoke_particle->x, smoke_particle->y, smoke_particle->radius, RGBA8(232, 232, 232, 200));
    }
}

void sprites_draw_player(float x, float y, float scale)
{

    // bottom fire
    float random_size = toolbox_random_float(scale, 5.0 * scale);
    vita2d_draw_line(x - 1.0 * scale, y, x, y + random_size, RGBA8(255, 0, 0, 200));
    vita2d_draw_line(x, y + random_size, x + 1.0 * scale, y, RGBA8(255, 0, 0, 200));
    
    //left side
    vita2d_draw_line(x - 2.0 * scale, y, x + 2.0 * scale, y, SHIP_PRIMARY_COLOR);
    vita2d_draw_line(x - 2.0 * scale, y, x - 1.0 * scale, y - 2.0 * scale, SHIP_PRIMARY_COLOR);
    vita2d_draw_line(x - 1.0 * scale, y - 2.0 * scale, x - 1.0 * scale, y - 4.0 * scale, SHIP_PRIMARY_COLOR);
    vita2d_draw_line(x - 1.0 * scale, y - 4.0 * scale, x, y - 5.0 * scale, SHIP_PRIMARY_COLOR);

    //right side
    vita2d_draw_line(x, y - 5.0 * scale, x + 1.0 * scale, y - 4.0 * scale, SHIP_PRIMARY_COLOR);
    vita2d_draw_line(x + 1.0 * scale, y - 4.0 * scale, x + 1.0 * scale, y - 2.0 * scale, SHIP_PRIMARY_COLOR);
    vita2d_draw_line(x + 1.0 * scale, y - 2.0 * scale, x + 2.0 * scale, y, SHIP_PRIMARY_COLOR);

}