#include <curses.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "cpu.h"
#include "decoder.h"
#include "invaders.h"

#define clock 500  //1 period in nanoseconds 500ns == 2MHz

int main(int argc, char** argv) {
    //open code
    cpustate cpu;
    cpu.memory = malloc(0x10000);
    cpu.inspace = malloc(0x100);
    cpu.outspace = malloc(0x100);

    if (load_file(cpu.memory, argv[1], 0) == 1) {
        printf("couldn't load file");
    }

    cpu.pc = 0;
    char instruction[30];
    byte op;
    int cycles;

    struct timespec delay = {0, clock};
    int cpu_cycles_per_display_cycle = 10e8 / (clock * 60);  //.333333...  number of cpu cycles per 60hz refresh rate at 2MHz
    int current_cpu_cycle = 0;

    reset_inputs(&cpu);

    twobyte shiftreg = 0;  //hardware bit shift register
    byte oldp4 = cpu.outspace[4];

    initscr();
    keypad(stdscr, TRUE);
    noecho();
    nodelay(stdscr, TRUE);
    cbreak();  //initialise curses

    while (1) {
        getinput(&cpu);
        decode(cpu.memory, cpu.pc, instruction);
        op = cpu.memory[cpu.pc];
        //printf("executing %-10s (opcode 0x%02x) from address $%04x \n", instruction, op, cpu.pc);
        cycles = execute(&cpu);

        delay.tv_nsec *= abs(cycles);
        nanosleep(&delay, NULL);
        delay.tv_nsec = clock;

        monitorshift(&cpu, &shiftreg, &oldp4);  //do the shift register

        if (cycles == -7) {
            printf("CPU halted\n");
            exit(1);
        };
        //printw("p1-%x p2-%x ", cpu.inspace[1], cpu.inspace[2]);
        reset_inputs(&cpu);
        current_cpu_cycle++;
        if (current_cpu_cycle > cpu_cycles_per_display_cycle) {
            current_cpu_cycle = 0;
            draw(&cpu);
            refresh();
            interrupt(&cpu, 2);  //end screen interrupt
        }
        if (current_cpu_cycle == cpu_cycles_per_display_cycle / 2) {
            interrupt(&cpu, 1);  //mid screen interrupt
        }
    };
    return 0;
}