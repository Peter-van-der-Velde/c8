#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "opcodes.h"
#include "dbg.h"
#include "chip8.h"

/*
	opcode functions
*/
void op_0000(uint16_t op)
{
	switch (op & 0x00FF) {
		// 0x00E0: Clears the screen
		case 0x00E0:
			memset(gfx, 0, sizeof(gfx));
			draw_flag = 1;
			pc += 2;
			break;

		// 0x00EE: Returns from subroutine
		case 0x00EE:
			sp--;
			pc = stack[sp];
			pc += 2; // go to the next instruction
			break;

		default:
			log_warn("Unknown opcode [0x%X]", op);
	}
}

// Jumps to address NNN.
void op_1000(uint16_t op)
{
	pc = op & 0x0FFF;
}

// Calls subroutine at NNN.
void op_2000(uint16_t op)
{
	stack[sp] = pc;
	sp++;
	pc = op & 0x0FFF;
}

// 0x3xkk
// Skip next instruction if Vx = kk.
void op_3000(uint16_t op)
{
	// shift 2 bits to the right (from: 0x3A2F to: 0x000A)
	uint16_t Vx = (op & 0x0F00) >> (2 * 4);
	uint16_t kk = op & 0x00FF;
	if (V[Vx] == kk) {
		pc += 2;
	}
	pc += 2;
}

// 0x4xkk
// Skip next instruction if Vx != kk.
void op_4000(uint16_t op)
{
	// shift 2 bits to the right (from: 0x4A2F to: 0x000A)
	uint16_t Vx = (op & 0x0F00) >> (2 * 4);
	uint16_t kk = op & 0x00FF;
	if (V[Vx] != kk) {
		pc += 2;
	}
	pc += 2;
}

// 5xy0
// Skip next instruction if Vx = Vy.
void op_5000(uint16_t op)
{
	uint16_t x = (op & 0x0F00) >> (2 * 4);
	uint16_t y = (op & 0x00F0) >> (1 * 4);

	if (V[x] == V[y])
		pc += 2;

	pc += 2;
}

// 6xkk
// Set Vx = kk.
void op_6000(uint16_t op)
{
	uint8_t x = (op & 0x0F00) >> (2 * 4);
	uint8_t kk = op & 0x00FF;

	V[x] = kk;

	pc += 2;
}

// 7xkk
// Set Vx = Vx + kk.
void op_7000(uint16_t op)
{
	uint16_t x = (op & 0x0F00) >> (2 * 4);
	uint16_t kk = op & 0x00FF;

	V[x] += kk;

	pc += 2;
}

// 8xyn
void op_8000(uint16_t op)
{
	uint16_t x = (op & 0x0F00) >> (2 * 4);
	uint16_t y = (op & 0x00F0) >> (1 * 4);

	switch (op & 0x000F) {
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
			if (V[x] + V[y] > 0xFF)
				V[0xF] = 1; // set VF to 1
			else
				V[0xF] = 0; // set VF to 0

			V[x] = V[x] + V[y];
			break;

		case 0x0005: // Set Vx = Vx - Vy,
			if (V[x] > V[y])
				V[0xF] = 1;
			else
				V[0xF] = 0;

			V[x] = V[x] - V[y];
			break;

		case 0x0006: // set Vx = Vx SHL 1;
			if ((V[x] & 0x0001) == 0x0001) // If the least-significant bit of Vx is 1
				V[0xF] = 1;
			else
				V[0xF] = 0;

			V[x] = V[x] >> 1;
			break;

		case 0x0007: // substract x from y (y - x)
			if (V[x] < V[y])
				V[0xF] = 1;
			else
				V[0xF] = 0;

			V[x] = V[y] - V[x];
			break;

		case 0x000E: // multipy Vx by 2
			if ((V[x] & 0x80) == 0x80) // If the most-significant bit of Vx is 1
				V[0xF] = 1;
			else
				V[0xF] = 0;

			V[x] = V[x] << 1;
			break;

		default:
			log_warn("Unknown opcode [0x%X]", op);
			break;
	}

	pc += 2;
}

//9xy0
// Skip next instruction if Vx != Vy.
void op_9000(uint16_t op)
{
	uint16_t x = (op & 0x0F00) >> (2 * 4);
	uint16_t y = (op & 0x00F0) >> (1 * 4);

	if (V[x] != V[y])
		pc += 2;

	pc += 2;
}

// Annn - LD I, addr
// Set I = nnn.
void op_A000(uint16_t op)
{
	uint16_t nnn = op & 0x0FFF;
	I = nnn;

	pc += 2;
}

// Bnnn - JP V0, addr
// Jump to location nnn + V0.
void op_B000(uint16_t op)
{
	pc = (op & 0x0FFF) + V[0];
}

// Cxkk
// Set Vx = random byte AND kk.
void op_C000(uint16_t op)
{
	uint16_t x = (op & 0x0F00) >> (2 * 4);
	uint16_t kk = op & 0x00FF;
	uint16_t r = rand() % 0xFF;

	V[x] = r & kk;

	pc += 2;
}

// Dxyn
// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
void op_D000(uint16_t op)
{
	uint16_t x = V[((op & 0x0F00) >> 8)];
	uint16_t y = V[((op & 0x00F0) >> 4)];
	uint16_t height = op & 0x000F;
	uint16_t pixel;

	V[0xF] = 0;
	for (unsigned char yline = 0; yline < height; yline++) {
		pixel = memory[I + yline];

		for (unsigned char xline = 0; xline < 8; xline++) {
			if ((pixel & (0x80 >> xline)) != 0) { // check if pixel value is 1

				if (x + xline > SCREEN_WIDTH || y + yline > SCREEN_WIDTH) { // check if x or y is out of bounds
					log_warn("game tried to acces out of bounds values");
					return;
				}
				if (gfx[(x + xline + ((y + yline) * 64))] == 1) {
					V[0xF] = 1;
				}
				gfx[x + xline + ((y + yline) * 64)] ^= 1;
			}
		}
	}

	draw_flag = 1;
	pc += 2;
}

// EX00
void op_E000(uint16_t op)
{
	uint16_t x = (op & 0x0F00) >> 8;

	switch (op & 0x00FF) {
		case 0x009E: // EX9E: Skips the next instruction if the key stored in Vx is pressed
			if (key[V[x]] != 0)
				pc += 2;

			pc += 2;
			break;

		case 0x00A1: // EXA1: Skips the next instruction if the key stored in Vx isn't pressed
			if (key[V[x]] == 0)
				pc += 2;

			pc += 2;
			break;

		default:
			log_warn("Unknown opcode [0x%X]", op);
			break;
	}
}

// FX00
void op_F000(uint16_t op)
{
	uint16_t x = (op & 0x0F00) >> 8;
	int key_press = 0;

	switch (op & 0x00FF) {
		case 0x0007: // FX07: Set Vx = delay timer value.
			V[x] = delay_timer;

			pc += 2;
			break;

		case 0x000A: // FX0A: Wait for a key press, store the value of the key in Vx.
			for (int i = 0; i < NUMBER_OF_KEYS; i++) {
				if (key[i] != 0) {
					V[x] = i;
					key_press = 1;
				}
			}

			if (!key_press)
				return;

			pc += 2;
			break;

		case 0x0015: // FX15: Set delay timer = Vx.
			delay_timer = V[x];

			pc += 2;
			break;

		case 0x0018: // FX18: Set sound timer = Vx.
			sound_timer = V[x];

			pc += 2;
			break;

		case 0x001E: // FX1E: The values of I and Vx are added, and the results are stored in I.
			if (V[x] + I > 0xFFF)
				V[0xF] = 1;
			else
				V[0xF] = 0;

			I = I + V[x];
			pc += 2;
			break;

		case 0x0029: // FX29: Set I = location of sprite for digit Vx.
			I = V[x] * 0x5;

			pc += 2;
			break;

		case 0x0033: // FX33: Store BCD representation of Vx in memory locations I, I+1, and I+2.
			memory[I] = V[x] / 100;
			memory[I + 1] = (V[x] / 10) % 10;
			memory[I + 2] = (V[x] % 100) % 10;
			pc += 2;
			break;

		case 0x0055: // FX55: Store registers V0 through Vx in memory starting at location I.
			for (int i = 0; i <= x; i++) {
				memory[i + I] = V[i];
			}

			if (ORIGINAL_0xF065_LOGIC)
				I = I + V[x] + 1;
			pc += 2;
			break;

		case 0x0065: // FX65: Read registers V0 through Vx from memory starting at location I.
			for (int i = 0; i <= x; i++) {
				V[i] = memory[i + I];
			}

			if (ORIGINAL_0xF065_LOGIC)
				I = I + V[x] + 1;
			pc += 2;
			break;

		default:
			log_warn("Unknown opcode [0x%X]", op);
			break;
	}
}
