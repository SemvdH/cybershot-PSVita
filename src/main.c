/*
Cybershot for PS Vita
Made by Sem van der Hoeven
*/

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
#include "toolbox/drawing.h"

#define printf psvDebugScreenPrintf

#define SCREEN_HEIGTH 544
#define SCREEN_WIDTH 960
#define SIMPLE_ENEMY_MAX_AMOUNT 20
#define ENEMY_MAX_AMOUNT 40
#define BULLET_MARGIN 5.0 // extra hitbox space to make sure bullets hit

#define MENU_SWITCH_DELAY 50	// delay in ms for when a menu screen is switched.
#define MENU_LEFT_RIGHT_NONE 0	// not left or right pressed
#define MENU_LEFT_RIGHT_LEFT 1	// left pressed
#define MENU_LEFT_RIGHT_RIGHT 2 // right pressed

/* 
menu windows:
|--------|	|------------|	|-----|
|tutorial|	|color select|	|start|
|--------|	|------------|	|-----|
*/
#define MENU_WINDOW_TUTORIAL 0
#define MENU_WINDOW_COLOR_SELECT 1
#define MENU_WINDOW_START 2

typedef enum
{
	START,
	MENU,
	GAME,
	GAMEOVER
} game_state;

typedef enum
{
	NONE,
	SHIP_SELECT,
	HOW_TO_PLAY
} menu_window_state;

uint8_t running, drawing_circle;
game_state current_state = START;

stick_data left_stick = {0, 0}, right_stick = {0, 0};
SceCtrlData pad;
uint8_t cross_pressed;

uint8_t current_bullet;
BULLET bullets[255];

uint8_t current_smoke_particle;
SMOKE_PARTICLE smoke_particles[255];

vita2d_pgf *pgf;
vita2d_pvf *pvf;

SceUInt64 deltaTime = 0; // delta time in ms
SceKernelSysClock sysclock;
timing_timer bullet_timer = {0, 250, 0};				  // 0 as starting time, 250 ms timeout, not elapsed
timing_timer menu_switch_input_delay_timer = {0, 500, 0}; // 0 as starting time, 500 ms timeout, not elapsed
timing_timer score_timer = {0, 100, 0};					  // timer to update score
timing_timer simple_enemy_spawn_timer = {0, 500, 0};	  // timer to spawn a new simple enemy
timing_timer complex_enemy_spawn_timer = {0, 2000, 0};	  // timer to spawn a new complex enemy

timing_timer start_color_switch_timer = {0, 500, 0}; // timer to switch color on the start menu
SceBool color_switch_value;							 // boolean to track if the title text should be colored or not

ENEMY_SPRITE enemies[ENEMY_MAX_AMOUNT];
uint32_t enemy_count;

int score;

float player_x, player_y, radius;

// ----------menu-----------
uint8_t ship_color_select_colors[3] = {255, 0, 255}; // colors to select ship
uint8_t menu_background_color[3] = {0, 255, 255};
char menu_background_color_index; // 1 or -1, is used to cycle between the colors of the menu

menu_window_state menu_active_window;
uint8_t menu_selected_window;		  // 0 is how to play, 1 is ship color select, 2 is game
uint8_t menu_left_right_pressed;	  // 0 is none, 1 is left, 2 is right
uint8_t last_menu_left_right_pressed; // what we pressed last update loop
uint8_t menu_needs_to_switch;
timing_timer menu_selected_window_input_delay_timer = {0, 300, 0}; // 0 as starting time, 300 ms timeout, not elapsed

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

/**
 * @brief initializes all variables
 */
void init_variables()
{
	running = 1;
	drawing_circle = 0;
	cross_pressed = 0;
	current_bullet = 0;
	current_smoke_particle = 0;
	enemy_count = 0;
	player_x = SCREEN_WIDTH / 2;
	player_y = 500;
	radius = 5.0;
	score = 0;
	color_switch_value = SCE_FALSE;
	ship_color_select_colors[0] = 0;
	ship_color_select_colors[1] = 255;
	ship_color_select_colors[2] = 255;
	menu_background_color[0] = 0;
	menu_background_color[1] = 255;
	menu_background_color_index = 1;
	menu_active_window = NONE;
	menu_selected_window = MENU_WINDOW_TUTORIAL;
	menu_left_right_pressed = MENU_LEFT_RIGHT_NONE;
	last_menu_left_right_pressed = MENU_LEFT_RIGHT_NONE;
	menu_needs_to_switch = 0;
}

// ################################################################
// ------------------------ GENERATE SPRITES ------------------
// ################################################################

/**
 * @brief initializes the sprites
 */
void init_sprites()
{
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

	// add simple enemies
	for (i = 0; i < SIMPLE_ENEMY_MAX_AMOUNT; i++)
	{
		ENEMY_SPRITE e = {NONACTIVE, SIMPLE, 0, 0, SIMPLE_ENEMY_COLOR, SIMPLE_ENEMY_MOVEMENT_SPEED, SIMPLE_ENEMY_SCORE};
		enemies[i] = e;
		enemy_count++;
	}
	for (i = SIMPLE_ENEMY_MAX_AMOUNT; i < ENEMY_MAX_AMOUNT; i++)
	{
		ENEMY_SPRITE e = {NONACTIVE, COMPLEX, 0, 0, COMPLEX_ENEMY_COLOR, COMPLEX_ENEMY_MOVEMENT_SPEED, COMPLEX_ENEMY_SCORE};
		enemies[i] = e;
		enemy_count++;
	}
}

/**
 * @brief generates a bullet
 * 
 */
void generate_bullet()
{
	// {1, x1_pos, y1_pos, RGBA8(100, 100, 0, 255)};
	bullets[current_bullet].active = ACTIVE;
	bullets[current_bullet].x = player_x;
	bullets[current_bullet].y = player_y - SHIP_HEIGHT;
	bullets[current_bullet].color = RGBA8(255, 100, 0, 255);
	current_bullet = (current_bullet + 1) % 254;
}

/**
 * @brief generates a smoke particle
 * 
 */
void generate_smoke_particle()
{
	smoke_particles[current_smoke_particle].active = ACTIVE;
	smoke_particles[current_smoke_particle].x = player_x;
	smoke_particles[current_smoke_particle].y = player_y - SHIP_HEIGHT;
	smoke_particles[current_smoke_particle].radius = SMOKE_START_RADIUS;
	smoke_particles[current_smoke_particle].explosion_direction = 1;
	current_smoke_particle = (current_smoke_particle + 1) % 254;
}

/**
 * @brief generates an enemy
 * 
 * @param enemy the enemy to generate
 */
void generate_enemy(ENEMY_SPRITE *enemy)
{
	enemy->active = ACTIVE;
	enemy->x = toolbox_random_float(0, SCREEN_WIDTH - 1);
	enemy->y = 0;
}
/**
 * @brief generates a simple enemy
 * 
 */
void generate_simple_enemy()
{
	for (int i = 0; i < SIMPLE_ENEMY_MAX_AMOUNT; i++)
	{
		if (enemies[i].active == NONACTIVE)
		{
			generate_enemy(&enemies[i]);
			break;
		}
	}
}

/**
 * @brief generates a complex enemy
 * 
 */
void generate_complex_enemy()
{
	for (int i = SIMPLE_ENEMY_MAX_AMOUNT; i < ENEMY_MAX_AMOUNT; i++)
	{
		if (enemies[i].active == NONACTIVE)
		{
			generate_enemy(&enemies[i]);
			break;
		}
	}
}

// ################################################################
// ------------------------ END GENERATE SPRITES ------------------
// ################################################################

// ################################################################
// ------------------------ COLLISION ------------------
// ################################################################

/**
 * @brief checks if a bullet has hit the given object
 * 
 * @param bullet the bullet to check for
 * @param x1 the x pos of the center of the other object
 * @param x2 the y pos of the center of the other object
 * @param width the width of the other object
 * @param heigth the heigth of the other object
 * @return uint8_t 0 if no collision, 1 if there is a collision.
 */
uint8_t bullet_is_collision(BULLET *bullet, float x, float y, float width, float heigth)
{
	return toolbox_is_collision(bullet->x + BULLET_WIDTH / 2.0, bullet->y + BULLET_HEIGTH / 2.0, BULLET_WIDTH + BULLET_MARGIN, BULLET_HEIGTH, x, y, width, heigth);
}

/**
 * @brief checks wether a bullet has hit an enemy
 * 
 * @param bullet the bulle to check for
 * @param enemy the enemy to check for
 * @return uint8_t 0 if no collision, 1 if there is a collision
 */
uint8_t bullet_hit_enemy(BULLET *bullet, ENEMY_SPRITE *enemy)
{
	if (enemy->enemy_type == SIMPLE)
	{
		return bullet_is_collision(bullet, enemy->x, enemy->y, SIMPLE_ENEMY_SIZE, SIMPLE_ENEMY_SIZE);
	}
	else if (enemy->enemy_type == COMPLEX)
	{
		return bullet_is_collision(bullet, enemy->x, enemy->y, COMPLEX_ENEMY_SIZE, COMPLEX_ENEMY_SIZE);
	}

	return 0;
}

uint8_t enemy_hit_player(ENEMY_SPRITE *enemy)
{
	if (enemy->enemy_type == SIMPLE)
	{
		return toolbox_is_collision(enemy->x, enemy->y, SIMPLE_ENEMY_SIZE, SIMPLE_ENEMY_SIZE, player_x, player_y - PLAYER_Y_OFFSET, PLAYER_WIDTH, PLAYER_HEIGTH);
	}
	else if (enemy->enemy_type == COMPLEX)
	{
		return toolbox_is_collision(enemy->x, enemy->y, COMPLEX_ENEMY_SIZE, COMPLEX_ENEMY_SIZE, player_x, player_y - PLAYER_Y_OFFSET, PLAYER_WIDTH, PLAYER_HEIGTH);
	}

	return 0;
}

/**
 * @brief checks the collision for all bullets
 * 
 */
void check_bullet_collisions()
{
	for (int e = 0; e < enemy_count; e++)
	{
		if (enemies[e].active == ACTIVE)
		{
			for (int b = 0; b < 255; b++)
			{
				if (bullets[b].active == ACTIVE)
				{
					if (bullet_hit_enemy(&bullets[b], &enemies[e]))
					{
						bullets[b].active = NONACTIVE;
						enemies[e].active = NONACTIVE;
						score += enemies[e].score;
						break;
					}
				}
			}
		}
	}
}

/**
 * @brief checks the collision for all enemies with the player
 * 
 * @returns true if there was a collision
 */
SceBool check_player_collisions()
{
	SceBool res = SCE_FALSE;
	for (int i = 0; i < enemy_count; i++)
	{
		if (enemies[i].active == ACTIVE)
		{
			if (enemy_hit_player(&enemies[i]))
			{
				res = SCE_TRUE;
				break;
			}
		}
	}
	return res;
}

// ################################################################
// ------------------------ END COLLISION ------------------
// ################################################################

void init()
{
	/* to enable analog sampling */
	sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);

	vita2d_init();
	vita2d_set_clear_color(RGBA8(0x40, 0x40, 0x40, 0xFF));
	init_variables();

	pgf = vita2d_load_default_pgf();
	pvf = vita2d_load_default_pvf();

	memset(&pad, 0, sizeof(pad));

	init_sprites();

	//TODO add other enemies
}

// ################################################################
// ------------------------ UPDATE FUNCTIONS ------------------
// ################################################################

void update_start()
{
	timing_update_timer(&menu_switch_input_delay_timer, deltaTime);
	timing_check_timer_elapsed(&menu_switch_input_delay_timer);
	timing_update_timer(&start_color_switch_timer, deltaTime);
	timing_check_timer_elapsed(&start_color_switch_timer);

	if (start_color_switch_timer.elapsed)
	{
		color_switch_value = !color_switch_value;
		start_color_switch_timer.elapsed = 0;
	}

	if (cross_pressed)
		if (menu_switch_input_delay_timer.elapsed)
		{
			current_state = MENU;
			menu_switch_input_delay_timer.elapsed = 0;
			player_x = 700;
			player_y = 300;
		}
}

void update_menu()
{
	timing_update_timer(&menu_switch_input_delay_timer, deltaTime);
	timing_check_timer_elapsed(&menu_switch_input_delay_timer);
	timing_update_timer(&menu_selected_window_input_delay_timer, deltaTime);
	timing_check_timer_elapsed(&menu_selected_window_input_delay_timer);

	if (menu_background_color[0] == 254 || menu_background_color[0] == 1)
	{
		menu_background_color[0] += menu_background_color_index;
		menu_background_color[1] -= menu_background_color_index;
	}
	else
	{
		menu_background_color[0] += menu_background_color_index * 2;
		menu_background_color[1] -= menu_background_color_index * 2;
	}

	if (menu_background_color[0] == 0 || menu_background_color[1] == 0)
		menu_background_color_index = -menu_background_color_index;

	if (cross_pressed)
		if (menu_switch_input_delay_timer.elapsed)
		{
			current_state = GAME;
			player_x = SCREEN_WIDTH / 2;
			player_y = 500;
		}

	if (menu_selected_window_input_delay_timer.elapsed)
	{
		if (menu_left_right_pressed == MENU_LEFT_RIGHT_LEFT) // left
		{
			menu_needs_to_switch = !menu_needs_to_switch;
			if (menu_needs_to_switch)
			{
				menu_selected_window -= 1;
				if (menu_selected_window == 255)
					menu_selected_window = MENU_WINDOW_START;
				menu_left_right_pressed = MENU_LEFT_RIGHT_NONE;
				menu_selected_window_input_delay_timer.elapsed = 0;
			}
		}

		if (menu_left_right_pressed == MENU_LEFT_RIGHT_RIGHT) // right
		{
			menu_needs_to_switch = !menu_needs_to_switch;
			if (menu_needs_to_switch)
			{
				menu_selected_window += 1;
				if (menu_selected_window == 3)
					menu_selected_window = MENU_WINDOW_TUTORIAL;
				menu_left_right_pressed = MENU_LEFT_RIGHT_NONE;
				menu_selected_window_input_delay_timer.elapsed = 0;
			}
		}
	}
}

void update_game()
{

	timing_update_timer(&bullet_timer, deltaTime); // update timer
	timing_check_timer_elapsed(&bullet_timer);
	timing_update_timer(&score_timer, deltaTime);
	timing_check_timer_elapsed(&score_timer);
	timing_update_timer(&simple_enemy_spawn_timer, deltaTime);
	timing_check_timer_elapsed(&simple_enemy_spawn_timer);
	timing_update_timer(&complex_enemy_spawn_timer, deltaTime);
	timing_check_timer_elapsed(&complex_enemy_spawn_timer);

	if (cross_pressed)
	{
		if (bullet_timer.elapsed)
		{
			generate_bullet();
			bullet_timer.elapsed = 0;
			generate_smoke_particle();
		}
	}

	if (abs(left_stick.x) > 15)
	{
		player_x += ctrl_input_calc_value(left_stick.x, deltaTime);
		//checks to make sure the player doesn't go behind the left and right bar
		if (player_x < 10)
			player_x = 10;
		if (player_x > SCREEN_WIDTH - 10)
			player_x = SCREEN_WIDTH - 10;
	}
	if (abs(left_stick.y) > 15)
	{
		player_y += ctrl_input_calc_value(left_stick.y, deltaTime);
		// checks to make sure the player doesn't go behind the top and bottom bar
		if (player_y > SCREEN_HEIGTH - 10)
			player_y = SCREEN_HEIGTH - 10;
		if (player_y < 30)
			player_y = 30;
	}

	if (player_x <= 0)
		player_x = 0;
	if (player_x >= SCREEN_WIDTH)
		player_x = SCREEN_WIDTH - 1;
	if (player_y <= 0)
		player_y = 0;
	if (player_y >= SCREEN_HEIGTH)
		player_y = SCREEN_HEIGTH - 1;

	if (check_player_collisions() == SCE_TRUE)
	{
		current_state = GAMEOVER;
		menu_switch_input_delay_timer.elapsed = 0;
		return;
	}

	if (score_timer.elapsed)
		score += 1;

	check_bullet_collisions();

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

	if (simple_enemy_spawn_timer.elapsed)
	{
		generate_simple_enemy();
		simple_enemy_spawn_timer.elapsed = 0;
	}

	if (complex_enemy_spawn_timer.elapsed)
	{
		generate_complex_enemy();
		complex_enemy_spawn_timer.elapsed = 0;
	}

	for (int i = 0; i < ENEMY_MAX_AMOUNT; i++)
	{
		if (enemies[i].active == ACTIVE)
		{
			enemies[i].y += enemies[i].movement_speed;
			if (enemies[i].y >= SCREEN_HEIGTH + COMPLEX_ENEMY_SIZE) // complex enemy is biggest
			{
				enemies[i].active = NONACTIVE;
			}
		}
	}
}

void update_gameover()
{
	timing_update_timer(&menu_switch_input_delay_timer, deltaTime);
	timing_check_timer_elapsed(&menu_switch_input_delay_timer);
	timing_update_timer(&start_color_switch_timer, deltaTime);
	timing_check_timer_elapsed(&start_color_switch_timer);

	if (start_color_switch_timer.elapsed)
	{
		color_switch_value = !color_switch_value;
		start_color_switch_timer.elapsed = 0;
	}

	if (cross_pressed)
		if (menu_switch_input_delay_timer.elapsed)
		{
			current_state = START;
			init_variables();
			init_sprites();
			menu_switch_input_delay_timer.elapsed = 0;
		}
}

void update()
{
	deltaTime = timing_get_deltatime(&sysclock);
	cross_pressed = 0;
	if (deltaTime < 0)
		shit_yourself();

	sceCtrlPeekBufferPositive(0, &pad, 1);

	if (pad.buttons & SCE_CTRL_START && pad.buttons & SCE_CTRL_SELECT)
		running = 0;
	if (pad.buttons & SCE_CTRL_CROSS)
		cross_pressed = 1;

	//TODO check on button release?
	if (pad.buttons & SCE_CTRL_LEFT)
		menu_left_right_pressed = MENU_LEFT_RIGHT_LEFT;
	if (pad.buttons & SCE_CTRL_RIGHT)
		menu_left_right_pressed = MENU_LEFT_RIGHT_RIGHT;

	ctrl_input_get_leftstick(&pad, &left_stick);

	// ctrl_input_get_rightstick(&pad, &right_stick);

	switch (current_state)
	{
	case START:
		update_start();
		break;

	case MENU:
		update_menu();
		break;

	case GAME:
		update_game();
		break;
	case GAMEOVER:
		update_gameover();
		break;
	}
}

// ################################################################
// ------------------------ END UPDATE FUNCTIONS ------------------
// ################################################################

// ################################################################
// ------------------------ DRAW FUNCTIONS ------------------
// ################################################################

void draw_start()
{
	unsigned int text_color;
	unsigned int background_color;
	if (color_switch_value == SCE_TRUE)
	{
		text_color = COLOR_BLACK;
		background_color = COLOR_CYAN;
	}
	else
	{
		background_color = COLOR_BLACK;
		text_color = COLOR_CYAN;
	}
	drawing_draw_window_filled(SCREEN_WIDTH / 2 - 300 / 2, 50, 300, 100, "Game Title", pgf, background_color);
	vita2d_pgf_draw_text(pgf, SCREEN_WIDTH / 2 - 300 / 2 + 47, 50 + 70, text_color, 2.0, "Cybershot");

	drawing_draw_window_filled(600, 400, 226, 80, "Message", pgf, SECONDARY_BORDER_COLOR); // width: 28 pixels for each character
	vita2d_pgf_draw_text(pgf, 622, 457, COLOR_BLACK, 1.2f, "Press X to start");
}

void draw_menu_window_tutorial(uint8_t sel_window)
{
	if (sel_window == MENU_WINDOW_TUTORIAL)
	{
		drawing_draw_selected_window_filled(100, 200, 226, 80, "Tutorial", pgf, SECONDARY_BORDER_COLOR);
	}
	else
	{
		drawing_draw_window_filled(100, 200, 226, 80, "Tutorial", pgf, SECONDARY_BORDER_COLOR);
	}

	vita2d_pgf_draw_text(pgf, 122, 257, COLOR_BLACK, 1.2f, "How to play");
}

void draw_menu_window_color_select(uint8_t sel_window)
{
	if (sel_window == MENU_WINDOW_COLOR_SELECT)
	{
		drawing_draw_selected_window_filled(200, 300, 226, 80, "Make it pretty", pgf, SECONDARY_BORDER_COLOR);
	}
	else
	{
		drawing_draw_window_filled(200, 300, 226, 80, "Make it pretty", pgf, SECONDARY_BORDER_COLOR);
	}

	vita2d_pgf_draw_text(pgf, 222, 357, COLOR_BLACK, 1.2f, "Ship color select");
}

void draw_menu_window_start(uint8_t sel_window)
{
	if (sel_window == MENU_WINDOW_START)
	{
		drawing_draw_selected_window_filled(600, 400, 226, 80, "Message", pgf, SECONDARY_BORDER_COLOR);
	}
	else
	{
		drawing_draw_window_filled(600, 400, 226, 80, "Message", pgf, SECONDARY_BORDER_COLOR); // width: 28 pixels for each character
	}

	vita2d_pgf_draw_text(pgf, 622, 457, COLOR_BLACK, 1.2f, "Start");
}

void draw_menu()
{
	// TODO add windows for tutorial and color select, and make "press x to start" window just a "start" window
	drawing_draw_window_filled(SCREEN_WIDTH / 2 - 212 / 2, 50, 212, 100, "Window Title", pgf, RGBA8(menu_background_color[0], menu_background_color[1], menu_background_color[2], 255));
	vita2d_pgf_draw_text(pgf, SCREEN_WIDTH / 2 - 212 / 2 + 47, 50 + 70, COLOR_BLACK, 2.0, "Menu");

	draw_menu_window_tutorial(menu_selected_window);
	draw_menu_window_color_select(menu_selected_window);
	draw_menu_window_start(menu_selected_window);

	// drawing_draw_window_filled(100, 200, 356, 120, "WIP", pgf, RGBA8(255, 0, 0, 255)); // width: 28 pixels for each character
	char buf[80];
	sprintf(buf, "sel menu %d", menu_selected_window);
	vita2d_pgf_draw_text(pgf, 10, 10, COLOR_MAGENTA, 1.2f, buf);

	//draw warning triangle
	// drawing_draw_triangle(320, 240, 280, 300, 360, 300, 3, COLOR_BLACK);
	// drawing_draw_vline(320 - 1.5, 255, 30, 6, COLOR_BLACK);
	// vita2d_draw_rectangle(320 - 1.5, 290, 6, 6, COLOR_BLACK);
}

void draw_game()
{
	sprites_draw_player(player_x, player_y, PLAYER_SCALE, RGBA8(ship_color_select_colors[0], ship_color_select_colors[1], ship_color_select_colors[2], 255));

	for (int i = 0; i < 255; i++)
	{
		sprites_draw_bullet(&bullets[i]);
		sprites_draw_smoke_circle(&smoke_particles[i]);
	}

	for (int i = 0; i < enemy_count; i++)
	{
		sprites_draw_enemy(&enemies[i]);
	}

	drawing_draw_window_filled(20, 40, 160, 80, "SCORE", pgf, SECONDARY_BORDER_COLOR);

	char score_text[15];
	sprintf(score_text, "%07d", score);
	vita2d_pgf_draw_text(pgf, 42, 97, COLOR_BLACK, 1.2f, score_text);

	char title_text[40];
	sprintf(title_text, "CYBERSHOT_PSVITA - FPS: %d", timing_get_fps(deltaTime));

	drawing_draw_window_clear(0, 0, SCREEN_WIDTH, SCREEN_HEIGTH, title_text, pgf);
}

void draw_gameover()
{

	unsigned int text_color;
	unsigned int background_color;
	if (color_switch_value == SCE_TRUE)
	{
		text_color = COLOR_BLACK;
		background_color = COLOR_RED;
	}
	else
	{
		background_color = COLOR_BLACK;
		text_color = COLOR_RED;
	}

	drawing_draw_window_filled(SCREEN_WIDTH / 2 - 300 / 2, 50, 300, 100, "You're dead", pgf, background_color);
	vita2d_pgf_draw_text(pgf, SCREEN_WIDTH / 2 - 300 / 2 + 47, 50 + 70, text_color, 2.0, "Game Over");

	drawing_draw_window_filled(300, 300, 160, 80, "SCORE", pgf, SECONDARY_BORDER_COLOR);

	char score_text[15];
	sprintf(score_text, "%07d", score);
	vita2d_pgf_draw_text(pgf, 322, 357, COLOR_BLACK, 1.2f, score_text);
}

void draw()
{
	vita2d_start_drawing();
	vita2d_clear_screen();

	switch (current_state)
	{
	case START:
		draw_start();
		break;

	case MENU:
		draw_menu();
		break;

	case GAME:
		draw_game();
		break;
	case GAMEOVER:
		draw_gameover();
		break;
	}

	vita2d_end_drawing();
	vita2d_swap_buffers();
}

// ################################################################
// ------------------------ END DRAW FUNCTIONS ------------------
// ################################################################

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
