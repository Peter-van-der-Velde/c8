#ifndef _memory_h
#define _memory_h

void initialize();
void read_rom(char *path);
void emulate_cycle();


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
extern unsigned char key[16];

extern char draw_flag;

#endif
