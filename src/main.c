#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>

#include <vita2d.h>

#include "sprites/sprites.h"
#include "timing/timing.h"

// 14 april 2021: 11:00-15:00

#define printf psvDebugScreenPrintf

// extern unsigned char _binary_image_png_start;

/**
 * @brief should be called when an unhandlable exception or error occurs. Triggers coredump.
 * 
 */
__attribute__((__noreturn__)) void shit_yourself(void)
{
	while (1)
	{
		*(int *)(0xAA) = 0x55; // trigger coredump
	}
}

int main(int argc, char *argv[])
{

	/* to enable analog sampling */
	sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
	SceCtrlData pad;
	vita2d_pgf *pgf;
	vita2d_pvf *pvf;
	// vita2d_texture *image;

	vita2d_init();
	vita2d_set_clear_color(RGBA8(0x40, 0x40, 0x40, 0xFF));

	pgf = vita2d_load_default_pgf();
	pvf = vita2d_load_default_pvf();

	/*
	 * Load the statically compiled image.png file.
	 */
	// image = vita2d_load_PNG_buffer(&_binary_image_png_start);

	memset(&pad, 0, sizeof(pad));

	BULLET b1 = {1, 200, 200, RGBA8(255, 0, 255, 255)};

	SceUInt64 deltaTime = 0; // delta time in ms
	SceKernelSysClock sysclock;

	float x1 = 300, y1 = 50, x2 = 400, y2 = 50;

	while (1)
	{
		deltaTime = timing_get_deltatime(&sysclock);
		if (deltaTime < 0)
			shit_yourself();

		sceCtrlPeekBufferPositive(0, &pad, 1);

		if (pad.buttons & SCE_CTRL_START)
			break;

		vita2d_start_drawing();
		vita2d_clear_screen();

		int16_t lxpos = -128 + pad.lx; // full left is 0 so -128 and full right is 255 so 128
		char leftStick[50] = "left stick: ";
		sprintf(leftStick, "left stick: %d", lxpos);

		vita2d_pgf_draw_text(pgf, 300, 30, RGBA8(255, 255, 0, 255), 1.0f, leftStick);
		if (abs(lxpos) > 15)
			x1 += (lxpos) * (deltaTime / 1000.0);
		char xtext[50];
		sprintf(xtext, "x: %f", x1);

		vita2d_pgf_draw_text(pgf, 200, 80, RGBA8(255, 0, 0, 255), 1.0f, xtext);

		if (x1 <= 0)
			x1 = 0;
		if (x1 > 700)
			x1 = 700;

		vita2d_draw_rectangle(x1, y1, (float)10, (float)10, RGBA8(100, 100, 100, 255));
		vita2d_draw_rectangle(x2, y2, (float)10, (float)10, RGBA8(169, 60, 23, 255));

		// vita2d_draw_texture(image, 100, 100);

		char text[80] = "process time: ";
		sprintf(text, "%lld ms", deltaTime);

		char fps[15] = "fps: ";
		sprintf(fps, "%d", timing_get_fps(deltaTime));

		vita2d_pgf_draw_text(pgf, 700, 30, RGBA8(0, 255, 0, 255), 1.0f, text);

		vita2d_pvf_draw_text(pvf, 700, 80, RGBA8(0, 255, 0, 255), 1.0f, fps);

		sprites_draw_bullet(&b1);

		vita2d_end_drawing();
		vita2d_swap_buffers();
	}

	/*
	 * vita2d_fini() waits until the GPU has finished rendering,
	 * then we can free the assets freely.
	 */
	vita2d_fini();
	// vita2d_free_texture(image);
	vita2d_free_pgf(pgf);
	vita2d_free_pvf(pvf);

	sceKernelExitProcess(0);
	return 0;
}
