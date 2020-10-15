#include <stdio.h>

#include "decoder.h"

int main(int argc, char** argv) {
    //open file
    FILE* code;
    code = fopen(argv[1], "rb");
    if (code == NULL) {
        printf("couldnt find file\n");
        return 1;
    }
    //get its length
    fseek(code, 0, SEEK_END);
    int size = ftell(code);
    fseek(code, 0, SEEK_SET);

    //load it into memory
    unsigned char memory[size + 1];
    fread(memory, size, 1, code);
    char buffer[30];
    int pc = 0;
    while (pc < size) {
        printf("%04x  ", pc);
        pc += decode(memory, pc, buffer);
        printf("%s\n", buffer);
    }

    return 0;
}
