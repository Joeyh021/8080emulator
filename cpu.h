#ifndef CPU_H
#define CPU_H

typedef unsigned char byte;
typedef unsigned short twobyte;

//condition codes
struct flags {
    byte sign : 1;
    byte zero : 1;
    byte aux : 1;
    byte parity : 1;
    byte carry : 1;
    byte pad : 3;
};

//the cpu, contains all registers and main memory
typedef struct {
    byte A;  //accumulator
    union {
        struct {
            byte C;
            byte B;
        };
        twobyte BC;
    };
    union {
        struct {
            byte E;
            byte D;
        };
        twobyte DE;
    };
    union {
        struct {
            byte L;
            byte H;
        };
        twobyte HL;
    };
    twobyte sp;           //stack pointer
    twobyte pc;           //program counter
    struct flags cc;      //condition codes
    byte int_enable : 1;  //enable interrupts
    byte* memory;         //main memory byte array
    byte* inspace;        //i/o space
    byte* outspace;
} cpustate;

//parity of a 2 byte number, 1 for even 0 for odd
static int parity(twobyte x);

//bitwise concatenate two bytes
static twobyte concat(byte high, byte low);

//show state of cpu, print register contents
void printstate(cpustate* cpu);

//load binary into memory at adr
int load_file(byte* memory, char* file, int adr);

//execute instruction
int execute(cpustate* cpu);

#endif