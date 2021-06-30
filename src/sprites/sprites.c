#include "sprites.h"

#define printf psvDebugScreenPrintf

void (*enemy_draw_functions[2])(ENEMY_SPRITE *enemy_sprite) = {sprites_draw_simple_enemy, sprites_draw_complex_enemy};

void sprites_draw_bullet(BULLET *bullet)
{
    if (bullet->active == ACTIVE)
    {
        vita2d_draw_rectangle(bullet->x, bullet->y, (float)BULLET_WIDTH, (float)BULLET_HEIGTH, bullet->color);
    }
}

void sprites_draw_smoke_circle(SMOKE_PARTICLE *smoke_particle)
{
    if (smoke_particle->active == ACTIVE)
    {
        vita2d_draw_fill_circle(smoke_particle->x, smoke_particle->y, smoke_particle->radius, RGBA8(232, 232, 232, 200));
    }
}

void sprites_draw_player(float x, float y, float scale)
{
    // bounding box: x -/+ 2.0 * scale, y - 5.0 * scale

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

void sprites_draw_enemy(ENEMY_SPRITE *enemy)
{
    if (enemy->active == ACTIVE)
    {
        (*enemy_draw_functions[enemy->enemy_type])(enemy);
    }
}

void sprites_draw_simple_enemy(ENEMY_SPRITE *enemy)
{
    vita2d_draw_line(enemy->x - 9.0, enemy->y, enemy->x, enemy->y + 9.0, enemy->color);
    vita2d_draw_line(enemy->x, enemy->y + 9.0, enemy->x + 9.0, enemy->y, enemy->color);
    vita2d_draw_line(enemy->x + 9.0, enemy->y, enemy->x, enemy->y - 9.0, enemy->color);
    vita2d_draw_line(enemy->x, enemy->y - 9.0, enemy->x - 9.0, enemy->y, enemy->color);
}

void sprites_draw_complex_enemy(ENEMY_SPRITE *enemy)
{

}