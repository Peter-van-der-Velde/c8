#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "dbg.h"
#include "opcodes.h"

unsigned short opcode;
unsigned char memory[4096];
unsigned char V[16];

unsigned char chip8_fontset[80] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

unsigned short I;
unsigned short pc;

unsigned char gfx[64 * 32];

unsigned char delay_timer;
unsigned char sound_timer;

unsigned short stack[16];
unsigned short sp;

unsigned char key[16];

char draw_flag = 0;

int original_0xF065_logic = 1;

void initialize()
{

	pc     = 0x200;  // Program counter starts at 0x200
	opcode = 0;      // Reset current opcode
	I      = 0;      // Reset index register
	sp     = 0;      // Reset stack pointer

	// Clear display
	for (int i = 0; i < (64*32); i++)
		gfx[i] = 0;

	// Clear stack
	for(int i = 0; i < 16; ++i)
		stack[i] = 0;

	// Clear registers V0-VF
	for(int i = 0; i < 16; ++i)
		V[i] = 0;

	// Clear memory
	for(int i = 0; i < 4096; ++i)
		memory[i] = 0;

	// Load fontset
	for(int i = 0; i < 80; ++i)
		memory[i] = chip8_fontset[i];

	// Reset timers
	delay_timer = 0;
	sound_timer = 0;

	// clear screen
	draw_flag = 1;
	srand(time(NULL));

	// // init
	// original_0xF065_logic;
}


void read_rom(char *path)
{
	FILE *ptr;
	size_t size;
	unsigned char *buffer;

	log_info("opening %s", path);
	// open file
	ptr = fopen(path,"rb");
	check(ptr, "There was an Error reading the file %s \n", path);

	fseek(ptr, 0, SEEK_END);
	size = ftell(ptr);         /*calc the size needed*/
	fseek(ptr, 0, SEEK_SET);

	buffer = malloc(size);  /*allocate space on heap*/
	check_mem(buffer);


	check((fread(buffer, sizeof buffer, size, ptr) != size), "there was an Error reading %s", path);

	log_info("Loaded ROM");

	for(u_int i = 0; i < size; ++i)
		memory[i + 512] = buffer[i];

	// clean up
	fclose(ptr);
	free(buffer);

	return;

error:
	free(buffer);
	fclose(ptr);
	exit(1);
}


void emulate_cycle()
{
	// Fetch opcode
	opcode = memory[pc] << 8 | memory[pc + 1];

	// Decode opcode
	switch(opcode & 0xF000)
	{

		case 0x0000: op_0000(opcode); break; // 0x00E0: Clears the screen [OR] 0x00EE: Returns from subroutine
		case 0x1000: op_1000(opcode); break; // Jumps to address NNN.
		case 0x2000: op_2000(opcode); break; // Calls subroutine at NNN.
		case 0x3000: op_3000(opcode); break; // Skip next instruction if Vx = kk.
		case 0x4000: op_4000(opcode); break; //
		case 0x5000: op_5000(opcode); break; //
		case 0x6000: op_6000(opcode); break; //
		case 0x7000: op_7000(opcode); break; //
		case 0x8000: op_8000(opcode); break; //
		case 0x9000: op_9000(opcode); break; //

		case 0xA000: op_A000(opcode); break; // ANNN: Sets I to the address NNN
		case 0xB000: op_B000(opcode); break; //
		case 0xC000: op_C000(opcode); break; //
		case 0xD000: op_D000(opcode); break; //
		case 0xE000: op_E000(opcode); break; //
		case 0xF000: op_F000(opcode); break; //

		default:
			log_warn("Unknown opcode: [0x%X]\n", opcode);
	}


}

void update_timers()
{
	if(delay_timer > 0)
		--delay_timer;

	if(sound_timer > 0)
	{
		if(sound_timer == 1)
			log_warn("Need to implement audio, Beep!");
		--sound_timer;
	}
}
