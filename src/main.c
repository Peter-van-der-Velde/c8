#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL.h>
#include "dbg.h"
#include "chip8.h"

int sdl_init(void);
void close_sdl(void);
void draw_gfx(void);

SDL_Window* c8_window = NULL;
SDL_Renderer* renderer = NULL;

int sdl_init(void)
{
	//Initialization flag
	int success = 1;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		log_err("SDL could not initialize! SDL_Error: %s", SDL_GetError());
		success = 0;
	} else {
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
			log_err("Warning: Linear texture filtering not enabled!");
		}

		c8_window = SDL_CreateWindow("c8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (c8_window == NULL) {
			log_err("Window could not be created! SDL_Error: %s", SDL_GetError());
			success = 0;
		} else {
			//Create renderer for window
			renderer = SDL_CreateRenderer(c8_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (renderer == NULL) {
				log_err("Renderer could not be created! SDL Error: %s", SDL_GetError());
				success = 0;
			} else {
				//Initialize renderer color
				SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
			}
		}
	}

	return success;
}

void close_sdl(void)
{
	//Destroy window
	SDL_DestroyWindow(c8_window);
	c8_window = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

void draw_gfx(void)
{
	int ww = SDL_GetWindowSurface(c8_window)->w;
	int wh = SDL_GetWindowSurface(c8_window)->h;
	int x_scale = ww / CHIP_8_SCREEN_WIDTH;
	int y_scale = wh / CHIP_8_SCREEN_HEIGHT;

	// clear renderer, reset it to white
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(renderer);

	int x;
	int y;
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
	for (unsigned int i = 0; i < sizeof(gfx); i++) {
		if (gfx[i] == 1) {
			x = (int)(i % CHIP_8_SCREEN_WIDTH);
			y = (int)(i / CHIP_8_SCREEN_HEIGHT / 2);
			SDL_Rect fill_rect = { x * x_scale, y * y_scale, x_scale, y_scale };
			SDL_RenderFillRect(renderer, &fill_rect);
		}
	}

	//Update screen
	SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[])
{
	if (argc < 2) {
		log_err("no file was specified");
		return 1;
	}

	initialize();
	read_rom(argv[1]);

	//Start up SDL and create window
	if (!sdl_init()) {
		log_err("Failed to initialize!");
	} else {
		int quit = 0;

		//Event handler
		SDL_Event e;

		//While application is running
		while (!quit) {
			// clean key_input, reset all key[] values to 0
			memset(key, 0, sizeof(key));

			//Handle events on queue
			while (SDL_PollEvent(&e) != 0) {
				if (e.type == SDL_QUIT) {
					quit = 1;
				} else if (e.type == SDL_KEYDOWN) {
					switch (e.key.keysym.sym) {
						case SDLK_0:
							key[0x0] = 1;
							break;

						case SDLK_1:
							key[0x1] = 1;
							break;

						case SDLK_2:
							key[0x2] = 1;
							break;

						case SDLK_3:
							key[0x3] = 1;
							break;

						case SDLK_4:
							key[0x4] = 1;
							break;

						case SDLK_5:
							key[0x5] = 1;
							break;

						case SDLK_6:
							key[0x6] = 1;
							break;

						case SDLK_7:
							key[0x7] = 1;
							break;

						case SDLK_8:
							key[0x8] = 1;
							break;

						case SDLK_9:
							key[0x9] = 1;
							break;

						case SDLK_a:
							key[0xA] = 1;
							break;

						case SDLK_b:
							key[0xB] = 1;
							break;

						case SDLK_c:
							key[0xC] = 1;
							break;

						case SDLK_d:
							key[0xD] = 1;
							break;

						case SDLK_e:
							key[0xE] = 1;
							break;

						case SDLK_f:
							key[0xF] = 1;
							break;

						default:
							break;
					}
				}
			}

			emulate_cycle();
			if (draw_flag) {
				draw_gfx();
			}

			update_timers();
		}
	}

	//Free resources and close SDL
	close_sdl();

	return 0;
}
