#ifndef SPRITES_H
#define SPRITES_H

//TODO draw main player sprite
typedef struct player_sprite_t
{
    /* data */
} PLAYER;

/**
 * @brief a struct that holds a bullet sprite, basically a rectangle
 *        there will be at most a few bullets available
 */
typedef struct bullet_sprite_t
{
    size_t active;      // whether or not the bullet should be drawn (0 or 1)
    float x;            // the x position
    float y;            // the y position
    unsigned int color; // color of the bullet
} BULLET;

/**
 * @brief function that draws the given bullet, if it is active
 * 
 * @param bullet the bullet struct pointer that points to the bullet that should be drawn
 */
void sprites_draw_bullet(BULLET *bullet);


#endif