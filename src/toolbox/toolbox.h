#ifndef TOOLBOX_H

#include <stdlib.h>
#include "../sprites/sprites.h"

/**
 * @brief generates a random float between the given floats
 * 
 * @param a the minumum value (inclusive)
 * @param b the maximum value (inclusive)
 */
float toolbox_random_float(float a, float b);

/**
 * @brief checks if a collision has happened between 2 ractancles (bounding boxes)
 * 
 * @param x1 the center x of the first object
 * @param y1 the center y of the first object
 * @param width1 the width of the first object
 * @param heigth1 the heigth of the first object
 * @param x2 the center x of the second object
 * @param y2 the center y of the second object
 * @param width2 the width of the second object
 * @param height2 the heigth of the second object
 * @return uint8_t 0 if no collision, 1 if there is a collision.
 */
uint8_t toolbox_is_collision(float x1, float y1, float width1, float heigth1, float x2, float y2, float width2, float height2);

#endif // !TOOLBOX_H