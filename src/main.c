#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "dbg.h"
#include "chip8.h"


int main(int argc, char *argv[])
{

	if(argc < 2) {
		log_err("no file was specified");
		return 1;
	}

	initialize();
	read_rom(argv[1]);

	int i = 0;
	while(1) {
		if (i % 10000 == 0)
			debug("cycle: %d", i++);
		emulate_cycle();
		if (draw_flag)
			render();
	}

	return 0;
}

