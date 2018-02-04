#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "dbg.h"
#include "chip8.h"

int main(int argc, char *argv[])
{

	if(argc < 2) {
		printf("no file was specified\n");
		return 1;
	}

	read_rom(argv[1]);

	return 0;
}

