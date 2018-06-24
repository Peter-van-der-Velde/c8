#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "dbg.h"
#include "chip8.h"

#define SCREEN_WIDTH = 640;
#define SCREEN_HEIGHT = 320;


int main(int argc, char *argv[])
{

	if(argc < 2) {
		printf("no file was specified\n");
		return 1;
	}

	initialize();
	read_rom(argv[1]);

	int i = 0;
	while(1) {
		if (i % 10000 == 0)
			debug("cycle: %d", i++);
		emulate_cycle();
	}

	return 0;
}

