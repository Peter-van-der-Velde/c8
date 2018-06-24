#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "dbg.h"
#include "chip8.h"


/*
	Opcodes functions
*/
void op_0000(unsigned short opcode)
{
	switch(opcode & 0x000F)
	{
		// 0x00E0: Clears the screen
		case 0x0000:
			// for(int i = 0; i < 2048; ++i)
				// gfx[i] = 0x0;
			memset(gfx, 0, sizeof(gfx));
			draw_flag = 1;
			break;

		// 0x00EE: Returns from subroutine
		case 0x000E:
			sp--;
			pc = stack[sp];
			pc += 2; // go to the next instruction
			break;

		default:
			log_warn("Unknown opcode [0x0000]: 0x%X\n", opcode);
	}
}


// Jumps to address NNN.
void op_1000(unsigned short opcode)
{
	pc = 0x0FFF & opcode;
}


// Calls subroutine at NNN.
void op_2000(unsigned short opcode)
{
	stack[sp] = pc;
	sp++;
	pc = opcode & 0x0FFF;
}


// 0x3xkk
// Skip next instruction if Vx = kk.
void op_3000(unsigned short opcode)
{
	// shift 2 bits to the right (from: 0x3A2F to: 0x000A)
	unsigned short Vx = (opcode & 0x0F00) >> (2*4);
	unsigned short kk = opcode & 0x00FF;
	if (V[Vx] == kk) {
		pc += 2;
	}
	pc += 2;
}


// 0x4xkk
// Skip next instruction if Vx != kk.
void op_4000(unsigned short opcode)
{
	// shift 2 bits to the right (from: 0x4A2F to: 0x000A)
	unsigned short Vx = (opcode & 0x0F00) >> (2*4);
	unsigned short kk = opcode & 0x00FF;
	if (V[Vx] != kk) {
		pc += 2;
	}
	pc += 2;
}


// 5xy0
// Skip next instruction if Vx = Vy.
void op_5000(unsigned short opcode)
{
	unsigned short x = (opcode & 0x0F00) >> (2 * 4);
	unsigned short y = (opcode & 0x00F0) >> (1 * 4);

	if (V[x] == V[y])
		pc += 2;

	pc += 2;
}


// 6xkk
// Set Vx = kk.
void op_6000(unsigned short opcode)
{
	unsigned short x = (opcode & 0x0F00) >> (2 * 4);
	unsigned short kk = opcode & 0x00FF;

	V[x] = kk;

	pc += 2;
}


// 7xkk
// Set Vx = Vx + kk.
void op_7000(unsigned short opcode)
{
	unsigned short x = (opcode & 0x0F00) >> (2 * 4);
	unsigned short kk = opcode & 0x00FF;

	V[x] = kk;

	pc += 2;
}


// 8xyn
void op_8000(unsigned short opcode)
{
	unsigned short x = (opcode & 0x0F00) >> (2 * 4);
	unsigned short y = (opcode & 0x00F0) >> (1 * 4);

	switch (opcode & 0x000F) {

		case 0x0000: // Set Vx = Vy.
			V[x] = V[y];
			break;
		case 0x0001: // Set Vx = Vx OR Vy.
			V[x] = V[x] | V[y];
			break;
		case 0x0002: // Set Vx = Vx AND Vy.
			V[x] = V[x] & V[y];
			break;
		case 0x0003: // Set Vx = Vx XOR Vy.
			V[x] = V[x] ^ V[y];
			break;
		case 0x0004: // Set Vx = Vx + Vy
			if (V[x] + V[y] > 255)
				V[15] = 1; // set VF to 1
			else
				V[15] = 0; // set VF to 0

			V[x] = V[x] + V[y];
			break;
		case 0x0005: // Set Vx = Vx - Vy,
			if (V[x] > V[y])
				V[15] = 1;
			else
				V[15] = 0;

			V[x] = V[x] - V[y];
			break;
		case 0x0006: // set Vx = Vx / 2;
			if ((V[x] & 0x0001) == 0x0001) // If the least-significant bit of Vx is 1
				V[15] = 1;
			else
				V[15] = 0;

			V[x] = V[x] / 2;
			break;
		case 0x0007: // substract x from y (y - -x)
			if (V[x] < V[y])
				V[15] = 1;
			else
				V[15] = 0;

			V[x] = V[y]	- V[x];
			break;
		case 0x000E: // multipy Vx by 2
			if ((V[x] & 0x8000) == 0x8000) // If the most-significant bit of Vx is 1
				V[15] = 1;
			else
				V[15] = 0;

			V[x] = V[x] * 2;
			break;

		default:
			log_warn("Unknown opcode [0x8000]: 0x%X\n", opcode);
			break;
	}

	pc += 2;
}


//9xy0
// Skip next instruction if Vx != Vy.
void op_9000(unsigned short opcode)
{
	unsigned short x = (opcode & 0x0F00) >> (2 * 4);
	unsigned short y = (opcode & 0x00F0) >> (1 * 4);

	if (V[x] != V[y])
		pc += 2;

	pc += 2;
}


// Annn - LD I, addr
// Set I = nnn.
void op_A000(unsigned short opcode)
{
	unsigned short nnn = opcode & 0x0FFF;
	I = nnn;

	pc += 2;
}


// Bnnn - JP V0, addr
// Jump to location nnn + V0.
void op_B000(unsigned short opcode)
{
	pc = (opcode & 0x0FFF) + V[0];
}


// Cxkk
// Set Vx = random byte AND kk.
void op_C000(unsigned short opcode)
{
	unsigned short x = (opcode & 0x0F00) >> (2 * 4);
	unsigned short kk = opcode & 0x00FF;
	unsigned short r = rand() % 256;

	V[x] = r & kk;

	pc += 2;
}


// Dxyn
// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
void op_D000(unsigned short opcode)
{
	unsigned short x = V[(opcode & 0x0F00) >> 8];
	unsigned short y = V[(opcode & 0x00F0) >> 4];
	unsigned short Vx = V[x];
	unsigned short Vy = V[y];
	unsigned short height = opcode & 0x000F;
	unsigned short pixel;

	V[15] = 0;
	for (int yline = 0; yline < height; yline++)
	{
		pixel = memory[I + yline];
		for(int xline = 0; xline < 8; xline++)
		{
			if((pixel & (0x80 >> xline)) != 0) // check if pixel value is 1
			{
				if(gfx[(Vx + xline + ((Vy + yline) * 64))] == 1)
				{
					V[15] = 1;
				}
				gfx[Vx + xline + ((Vy + yline) * 64)] ^= 1;
			}
		}
	}

	draw_flag = 1;
	pc += 2;
}


void op_E000(unsigned short opcode)
{
	unsigned short x = (opcode & 0x0F00) >> 8;

	switch(opcode & 0x00FF)
	{
		case 0x009E: // EX9E: Skips the next instruction if the key stored in Vx is pressed
			if(key[V[x]] != 0)
				pc += 2;

			pc += 2;
			break;

		case 0x00A1: // EXA1: Skips the next instruction if the key stored in Vx isn't pressed
			if(key[V[x]] == 0)
				pc += 2;

			pc += 2;
			break;

		default:
			log_warn("Unknown opcode [0xE000]: 0x%X\n", opcode);
			break;
	}

}


void op_F000(unsigned short opcode)
{
	log_warn("Unknown opcode [0x0000]: 0x%X\n", opcode);

	pc += 2;
}
