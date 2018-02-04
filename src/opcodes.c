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
		case 0x0000: // 0x00E0: Clears the screen 
			memset(gfx, 0, sizeof(gfx));       
			break;

		case 0x000E: // 0x00EE: Returns from subroutine          
			sp--;
			pc = stack[sp];
			pc += 2;
			break;

		default:
			log_warn("Unknown opcode [0x0000]: 0x%X\n", opcode);          
	}
}

void op_1000(unsigned short opcode)
{
	pc = 0x0FFF & opcode;
}

void op_2000(unsigned short opcode)
{
	stack[sp] = pc;
	sp++;
	pc = opcode & 0x0FFF;
}

void op_3000(unsigned short opcode)
{
	I = opcode & 0x0FFF;
	pc += 2;
}

void op_4000(unsigned short opcode)
{
	I = opcode & 0x0FFF;
	pc += 2;
}

void op_5000(unsigned short opcode)
{
	I = opcode & 0x0FFF;
	pc += 2;
}

void op_6000(unsigned short opcode)
{
	I = opcode & 0x0FFF;
	pc += 2;
}

void op_7000(unsigned short opcode)
{
	I = opcode & 0x0FFF;
	pc += 2;
}

void op_8000(unsigned short opcode)
{
	I = opcode & 0x0FFF;
	pc += 2;
}

void op_9000(unsigned short opcode)
{
	I = opcode & 0x0FFF;
	pc += 2;
}

void op_A000(unsigned short opcode)
{
	I = opcode & 0x0FFF;
	pc += 2;
}

void op_B000(unsigned short opcode)
{
	I = opcode & 0x0FFF;
	pc += 2;
}

void op_C000(unsigned short opcode)
{
	I = opcode & 0x0FFF;
	pc += 2;
}


void op_D000(unsigned short opcode)
{
	I = opcode & 0x0FFF;
	pc += 2;
}

void op_E000(unsigned short opcode)
{
	I = opcode & 0x0FFF;
	pc += 2;
}

void op_F000(unsigned short opcode)
{
	I = opcode & 0x0FFF;
	pc += 2;
}