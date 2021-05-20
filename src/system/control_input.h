#ifndef CONTROL_INPUT_H
#define CONTROL_INPUT_H

#include <stdio.h>
#include <psp2/ctrl.h>

/**
 * @brief a struct that holds the x and y pos for a stick. < 0 is left or down, > 0 is right or up. Value goes from 0 to 128
 * 
 */
typedef struct 
{
    int16_t x;
    int16_t y;
} stick_data;

/**
 * @brief gets the value for the left stick
 * NOTE: should be called after calling sceCtrlPeekBufferPositive
 * 
 * @param pad the sce control data pointer
 * @param stickdata the struct that will hold the result
 */
void ctrl_input_get_leftstick(SceCtrlData *pad, stick_data* stickdata);

/**
 * @brief gets the value for the right stick
 * NOTE: should be called after calling sceCtrlPeekBufferPositive
 * 
 * @param pad the sce control data pointer
 * @param stickdata the struct that will hold the result
 * @return stick_data the data of the stick in the struct format
 */
void ctrl_input_get_rightstick(SceCtrlData *pad, stick_data* stickdata);

/**
 * @brief calculates the value of the stick based on the input value
 * 
 * @param pos the position to calculate with
 * @param deltaTime the deltatime
 * @return float the value that can be used to change an entity.
 */
float ctrl_input_calc_value(int16_t pos, SceUInt64 deltaTime);
#endif