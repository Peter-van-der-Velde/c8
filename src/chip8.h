#ifndef _memory_h
#define _memory_h

#define CHIP_8_SCREEN_WIDTH 64
#define CHIP_8_SCREEN_HEIGHT 32

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 320
#define ORIGINAL_0xF065_LOGIC 1
#define NUMBER_OF_KEYS 16

void initialize();
void read_rom(char *path);
void emulate_cycle();
void update_timers();

extern unsigned short opcode;
extern unsigned char memory[4096];
extern unsigned char V[16];

extern unsigned short I;
extern unsigned short pc;

extern unsigned char delay_timer;
extern unsigned char sound_timer;

extern unsigned short stack[16];
extern unsigned short sp;

extern unsigned char gfx[64 * 32];
extern unsigned char key[NUMBER_OF_KEYS];

extern char draw_flag;

#endif
