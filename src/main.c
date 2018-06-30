#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "dbg.h"
#include "chip8.h"

int sdl_init();
void close_sdl();

SDL_Window* c8_window = NULL;
SDL_Surface* screen_surface = NULL;
SDL_Surface* current_surface = NULL;

int sdl_init()
{
	//Initialization flag
	int success = 1;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		log_err("SDL could not initialize! SDL_Error: %s", SDL_GetError());
		success = 0;
	}
	else
	{
		//Create window
		c8_window = SDL_CreateWindow( "c8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( c8_window == NULL )
		{
			log_err("Window could not be created! SDL_Error: %s", SDL_GetError());
			success = 0;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface( c8_window );
		}
	}

	return success;
}

void close_sdl()
{
	//Deallocate surface
	SDL_FreeSurface( current_surface );
	current_surface = NULL;

	//Destroy window
	SDL_DestroyWindow( c8_window );
	c8_window = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}


int main(int argc, char *argv[])
{

	if(argc < 2) {
		log_err("no file was specified");
		return 1;
	}

	initialize();
	read_rom(argv[1]);

	//Start up SDL and create window
	if(!sdl_init()) {
		log_err( "Failed to initialize!");
	}
	else {
		debug("else");
		//Load media
		//Main loop flag
		int quit = 0;

		//Event handler
		SDL_Event e;

		//While application is running
		while( !quit ) {
			// clean key_input, reset all key[] values to 0
			memset(key, 0, sizeof(key));

			//Handle events on queue
			while( SDL_PollEvent( &e ) != 0 ) {
				if( e.type == SDL_QUIT ) {
					quit = 1;
				}
				else if( e.type == SDL_KEYDOWN ) {

					debug("keydown");
					switch( e.key.keysym.sym ) {
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

			//Update the surface
			SDL_UpdateWindowSurface( c8_window );
		}
	}

	// int i = 0;
	// while(1) {
	// 	if (i % 10000 == 0)
	// 		debug("cycle: %d", i++);
	// 	emulate_cycle();
	// 	// if (draw_flag)
	// 	// 	render();
	// }

	//Free resources and close SDL
	close_sdl();

	return 0;
}

