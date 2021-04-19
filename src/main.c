#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>

#include <vita2d.h>

#include "sprites/sprites.h"

// 14 april 2021: 11:00-15:00

#define printf psvDebugScreenPrintf

// extern unsigned char _binary_image_png_start;

int main(int argc, char *argv[])
{

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

	BULLET b1 = {1,200,200,RGBA8(255,0,255,255)};

	while (1)
	{
		sceCtrlPeekBufferPositive(0, &pad, 1);

		if (pad.buttons & SCE_CTRL_START)
			break;

		vita2d_start_drawing();
		vita2d_clear_screen();

		// vita2d_draw_texture(image, 100, 100);

		vita2d_pgf_draw_text(pgf, 700, 30, RGBA8(0,255,0,255), 1.0f, "PGF Font sample!");

		vita2d_pvf_draw_text(pvf, 700, 80, RGBA8(0,255,0,255), 1.0f, "PVF Font sample!");

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
