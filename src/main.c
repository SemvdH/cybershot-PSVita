#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>
#include <sys/types.h>

#include <vita2d.h>

#include "sprites/sprites.h"
#include "system/timing.h"
#include "system/control_input.h"

// 14 april 2021: 11:00-15:00

#define printf psvDebugScreenPrintf

#define SCREEN_HEIGTH 544
#define SCREEN_WIDTH 940

size_t running = 1, drawing_circle = 0;

stick_data left_stick = {0, 0}, right_stick = {0, 0};
SceCtrlData pad;
uint8_t cross_pressed = 0;

uint8_t current_bullet = 0;
BULLET bullets[255];

uint8_t current_smoke_particle = 0;
SMOKE_PARTICLE smoke_particles[255];

vita2d_pgf *pgf;
vita2d_pvf *pvf;

SceUInt64 deltaTime = 0; // delta time in ms
SceKernelSysClock sysclock;
timing_timer bullet_timer = {0, 250, 0}; // 0 as starting time, 400 ms timeout, not elapsed

timer_t bullt = 0;
ENEMY_SPRITE simple_enemies[SIMPLE_ENEMY_MAX_AMOUNT];

float player_x = 300, player_y = 50, x2_pos = 400, y2_pos = 50, radius = 5.0;

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

void generate_bullet()
{
	// {1, x1_pos, y1_pos, RGBA8(100, 100, 0, 255)};
	bullets[current_bullet].active = ACTIVE;
	bullets[current_bullet].x = player_x;
	bullets[current_bullet].y = player_y - SHIP_HEIGHT;
	bullets[current_bullet].color = RGBA8(255, 100, 0, 255);
	current_bullet = (current_bullet + 1) % 254;
}

void generate_smoke_particle()
{
	smoke_particles[current_smoke_particle].active = ACTIVE;
	smoke_particles[current_smoke_particle].x = player_x;
	smoke_particles[current_smoke_particle].y = player_y - SHIP_HEIGHT;
	smoke_particles[current_smoke_particle].radius = SMOKE_START_RADIUS;
	smoke_particles[current_smoke_particle].explosion_direction = 1;
	current_smoke_particle = (current_smoke_particle + 1) % 254;
}

void init()
{
	/* to enable analog sampling */
	sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);

	vita2d_init();
	vita2d_set_clear_color(RGBA8(0x40, 0x40, 0x40, 0xFF));

	pgf = vita2d_load_default_pgf();
	pvf = vita2d_load_default_pvf();

	memset(&pad, 0, sizeof(pad));
	int i;
	for (i = 0; i < 255; i++)
	{
		BULLET temp = {NONACTIVE, 0, 100, RGBA8(0, i, 255, 255), 300.0};
		bullets[i] = temp;
	}

	for (i = 0; i < 255; i++)
	{
		SMOKE_PARTICLE s = {NONACTIVE, 0, 0, SMOKE_START_RADIUS};
		smoke_particles[i] = s;
	}

	for (i = 0; i < SIMPLE_ENEMY_MAX_AMOUNT; i++)
	{
		ENEMY_SPRITE e = {ACTIVE, SIMPLE, 20 * i + 10, 10, RGBA8(245, 90, 66, 255), 1.0};
		simple_enemies[i] = e;
	}
}

void update()
{
	deltaTime = timing_get_deltatime(&sysclock);
	cross_pressed = 0;
	if (deltaTime < 0)
		shit_yourself();

	sceCtrlPeekBufferPositive(0, &pad, 1);

	if (pad.buttons & SCE_CTRL_START)
		running = 0;
	if (pad.buttons & SCE_CTRL_CROSS)
		cross_pressed = 1;

	timing_update_timer(&bullet_timer, deltaTime); // update timer
	timing_check_timer_elapsed(&bullet_timer);

	ctrl_input_get_leftstick(&pad, &left_stick);
	// ctrl_input_get_rightstick(&pad, &right_stick);

	if (abs(left_stick.x) > 15)
		player_x += ctrl_input_calc_value(left_stick.x, deltaTime);
	if (abs(left_stick.y) > 15)
		player_y += ctrl_input_calc_value(left_stick.y, deltaTime);

	if (player_x <= 0)
		player_x = 0;
	if (player_x >= SCREEN_WIDTH)
		player_x = SCREEN_WIDTH - 1;
	if (player_y <= 0)
		player_y = 0;
	if (player_y >= SCREEN_HEIGTH)
		player_y = SCREEN_HEIGTH - 1;

	if (cross_pressed)
	{
		if (bullet_timer.elapsed)
		{
			generate_bullet();
			bullet_timer.elapsed = 0;
			generate_smoke_particle();
		}
	}

	for (int i = 0; i < 255; i++)
	{
		bullets[i].y -= bullets[i].movement_speed * (deltaTime / 1000.0);

		if (bullets[i].y <= 0)
		{
			bullets[i].active = 0;
		}

		smoke_particles[i].radius += smoke_particles[i].explosion_direction * (SMOKE_DISSAPPEAR_SPEED * (deltaTime / 1000.0)) * (SMOKE_MAX_RADIUS / smoke_particles[i].radius);

		if (smoke_particles[i].radius >= SMOKE_MAX_RADIUS)
			smoke_particles[i].explosion_direction = -1;

		if (smoke_particles[i].radius <= 0)
			smoke_particles[i].active = 0;
	}
}

void draw()
{
	vita2d_start_drawing();
	vita2d_clear_screen();

	sprites_draw_player(player_x, player_y, PLAYER_SCALE);
	// vita2d_draw_rectangle(x2_pos, y2_pos, (float)10, (float)10, RGBA8(169, 60, 23, 255));

	char text[80] = "process time: ";
	sprintf(text, "%f", simple_enemies[5].x);

	char fps[15] = "fps: ";
	sprintf(fps, "%d", timing_get_fps(deltaTime));

	vita2d_pgf_draw_text(pgf, 700, 30, RGBA8(0, 255, 0, 255), 1.0f, text);

	vita2d_pvf_draw_text(pvf, 700, 80, RGBA8(0, 255, 0, 255), 1.0f, fps);

	char timertext[100];
	sprintf(timertext, "time %lu", bullet_timer.time);
	vita2d_pgf_draw_text(pgf, 10, 30, RGBA8(0, 255, 150, 255), 1.0f, timertext);

	for (int i = 0; i < 255; i++)
	{
		sprites_draw_bullet(&bullets[i]);
		sprites_draw_smoke_circle(&smoke_particles[i]);
	}

	for (int i = 0; i < SIMPLE_ENEMY_MAX_AMOUNT; i++)
	{
		sprites_draw_enemy(&simple_enemies[i]);
	}

	vita2d_end_drawing();
	vita2d_swap_buffers();
}

int main(int argc, char *argv[])
{
	init();

	while (running)
	{
		update();
		draw();
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
