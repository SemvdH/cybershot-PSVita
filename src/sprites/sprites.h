#ifndef SPRITES_H
#define SPRITES_H

#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>

#include <vita2d.h>

#include <stdlib.h>

#include "../toolbox/toolbox.h"

#define SHIP_SECONDARY_COLOR RGBA8(153, 153, 153, 255)
#define SHIP_PRIMARY_COLOR RGBA8(0, 255, 195, 255)

#define SHIP_WIDTH 12;  // 3 * 4
#define SHIP_HEIGHT 15; // 3 * 5

#define SMOKE_START_RADIUS 8.0
#define SMOKE_MAX_RADIUS 10.0
#define SMOKE_DISSAPPEAR_SPEED 35.0

/**
 * @brief a struct that holds a bullet sprite, basically a rectangle
 *        there will be at most a few bullets available
 */
typedef struct bullet_sprite_t
{
    size_t active;        // whether or not the bullet should be drawn (0 or 1)
    float x;              // the x position
    float y;              // the y position
    unsigned int color;   // color of the bullet
    float movement_speed; // speed of the bullet (how much it should move each frame)
} BULLET;

typedef struct smoke_particle_t
{
    size_t active;              // whether or not to draw the smoke circle (0 or 1)
    float x;                    // x position
    float y;                    // y position
    float radius;               // radius of circle
    int8_t explosion_direction; // wether the explosion is expanding or shrinking.
                                // 1 = expanding, -1 = shrinking
} SMOKE_PARTICLE;

/**
 * @brief function that draws the given bullet, if it is active
 * 
 * @param bullet the bullet struct pointer that points to the bullet that should be drawn
 */
void sprites_draw_bullet(BULLET *bullet);

/**
 * @brief draws the player sprite
 * 
 * @param x the x position of the player
 * @param y the y position of the player
 * @param scale the scale of the ship
 */
void sprites_draw_player(float x, float y, float scale);

/**
 * @brief draws a smoke particle
 * 
 * @param smoke_particle the smoke particle to draw
 */
void sprites_draw_smoke_circle(SMOKE_PARTICLE *smoke_particle);

#endif