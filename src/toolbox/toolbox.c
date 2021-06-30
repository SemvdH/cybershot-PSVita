#include "toolbox.h"

float toolbox_random_float(float a, float b)
{
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

size_t toolbox_is_collision(float x1, float y1, float width1, float heigth1, float x2, float y2, float width2, float height2)
{
    //TODO make it work
    return x1 < x2 + width2 / 2 &&
           x1 + width1 / 2 > x2 &&
           y1 < y2 + height2 / 2 &&
           y1 + heigth1 / 2 > y2;
}