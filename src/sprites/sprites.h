#ifndef SPRITES_H
#define SPRITES_H

#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>

#include <vita2d.h>

#include <stdlib.h>

#include "../toolbox/toolbox.h"

#define SHIP_SECONDARY_COLOR RGBA8(153, 153, 153, 255)
#define SHIP_PRIMARY_COLOR RGBA8(0, 255, 195, 255)

#define SHIP_WIDTH 4
#define SHIP_HEIGHT 5

#define BULLET_WIDTH 3
#define BULLET_HEIGTH 8

#define PLAYER_SCALE 3.0
#define PLAYER_WIDTH (2.0 * 2 * PLAYER_SCALE)
#define PLAYER_HEIGTH (5.0 * PLAYER_SCALE)
#define PLAYER_Y_OFFSET (PLAYER_HEIGTH / 2) // the center y of the player is in the middle

#define SIMPLE_ENEMY_SIZE 9.0
#define SIMPLE_ENEMY_MOVEMENT_SPEED 3.0
#define SIMPLE_ENEMY_COLOR (RGBA8(245, 90, 66, 255))
#define SIMPLE_ENEMY_SCORE 100

#define COMPLEX_ENEMY_SIZE 12.0
#define COMPLEX_ENEMY_MOVEMENT_SPEED 5.0
#define COMPLEX_ENEMY_COLOR (RGBA8(177, 235, 52,220))
#define COMPLEX_ENEMY_SECONDARY_COLOR (RGBA8(152, 83, 181, 255))
#define COMPLEX_ENEMY_SCORE 500

#define SMOKE_START_RADIUS 8.0
#define SMOKE_MAX_RADIUS 10.0
#define SMOKE_DISSAPPEAR_SPEED 35.0

typedef enum
{
    SIMPLE,
    COMPLEX
} ENEMY_TYPE;

typedef enum
{
    NONACTIVE,
    ACTIVE
} SPRITE_ACTIVE;

/**
 * @brief a struct that holds a bullet sprite, basically a rectangle.
 *        there will be at most a few bullets available
 */
typedef struct sprite_t
{
    SPRITE_ACTIVE active; // whether or not the sprite should be drawn (0 or 1)
    float x;              // the top left x position
    float y;              // the top left y position
    unsigned int color;   // color of the sprite
    float movement_speed; // speed of the sprite (how much it should move each frame)
} BULLET;

typedef struct enemy_sprite_t
{
    SPRITE_ACTIVE active;  // whether or not the sprite should be drawn (0 or 1)
    ENEMY_TYPE enemy_type; // type of enemy
    float x;               // the center x position
    float y;               // the center y position
    unsigned int color;    // color of the sprite
    float movement_speed;  // speed of the sprite (how much it should move each frame)
    int score;             // score you get when you kill an enemy
} ENEMY_SPRITE;

typedef struct smoke_particle_t
{
    SPRITE_ACTIVE active;       // whether or not to draw the smoke circle (0 or 1)
    float x;                    // center x position
    float y;                    // center y position
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
 * @param color the color of the ship (rgb array)
 */
void sprites_draw_player(float x, float y, float scale, unsigned int color);

/**
 * @brief draws a smoke particle
 * 
 * @param smoke_particle the smoke particle to draw
 */
void sprites_draw_smoke_circle(SMOKE_PARTICLE *smoke_particle);

/**
 * @brief draws an enemy sprite
 * 
 * @param enemy_sprite the sprite to draw
 */

void sprites_draw_enemy(ENEMY_SPRITE *enemy_sprite);

/**
 * @brief draws the simple enemy
 * 
 * @param enemy_sprite the enemy sprite struct
 */
void sprites_draw_simple_enemy(ENEMY_SPRITE *enemy);

/**
 * @brief draws the complex enemy
 * 
 * @param enemy_sprite the enemy sprite struct
 */
void sprites_draw_complex_enemy(ENEMY_SPRITE *enemy);

#endif