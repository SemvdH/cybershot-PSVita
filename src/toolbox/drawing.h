#ifndef DRAWING_H
#define DRAWING_H

#include <vita2d.h>

// /**
//  * @brief draws a line with a specified width
//  * 
//  * @param x0 the start x coordinate of the line
//  * @param y0 the start y coordinate of the line
//  * @param x1 the end x coordinate of the line
//  * @param y1 the end y coordinate of the line
//  * @param thiccness the width of the line
//  * @param color the color of the line
//  */
// void drawing_draw_line_thiccness(float x0, float y0, float x1, float y1, int thiccness, unsigned int color);

/**
 * @brief draws an open rectangle
 * 
 * @param x the top left x position of the rectangle
 * @param y the top left y position of the rectangle
 * @param width the width of the rectangle
 * @param heigth the heigth of the rectangle
 * @param thiccness the width of the lines of the rectangle
 * @param color the color of the rectangle
 */
void drawing_draw_rectangle_open(float x, float y, float width, float heigth, int thiccness, unsigned int color);

#endif // !DRAWING_H
