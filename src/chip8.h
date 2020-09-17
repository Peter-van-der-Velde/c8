#ifndef _memory_h
#define _memory_h
#include <inttypes.h>

#define CHIP_8_SCREEN_WIDTH 64
#define CHIP_8_SCREEN_HEIGHT 32

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 320
#define ORIGINAL_0xF065_LOGIC 1
#define NUMBER_OF_KEYS 16

void initialize(void);
void read_rom(char* path);
void emulate_cycle(void);
void update_timers(void);

extern uint16_t opcode;
extern uint8_t memory[4096];
extern uint8_t V[16];

extern uint16_t I;
extern uint16_t pc;

extern uint8_t delay_timer;
extern uint8_t sound_timer;

extern uint16_t stack[16];
extern uint16_t sp;

extern uint8_t gfx[64 * 32];
extern uint8_t key[NUMBER_OF_KEYS];

extern char draw_flag;

#endif
