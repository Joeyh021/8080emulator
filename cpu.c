#include "cpu.h"

#include <stdio.h>
#include <stdlib.h>

static int parity(twobyte x) {
    int i;
    int total = 0;
    for (i = 0; i < 16; i++) {
        total += ((x >> i) & 1);
    }
    return (!(total % 2));
}
int load_file(byte* memory, char* filename, int adr) {
    //open file
    FILE* code = fopen(filename, "rb");
    if (code == NULL) {
        return 1;
    }

    //get length
    fseek(code, 0, SEEK_END);
    int size = ftell(code);
    fseek(code, 0, SEEK_SET);

    //load into memory
    fread(&(memory[adr]), size, 1, code);
}

static twobyte concat(byte high, byte low) {
    return ((high << 8) | low);
}

void printstate(cpustate* cpu) {
    printf("--registers--\n");
    printf("A - 0x%02x\n", cpu->A);
    printf("B - 0x%02x\n", cpu->B);
    printf("C - 0x%02x\n", cpu->C);
    printf("D - 0x%02x\n", cpu->D);
    printf("E - 0x%02x\n", cpu->E);
    printf("H - 0x%02x\n", cpu->H);
    printf("L - 0x%02x\n", cpu->L);
    printf("SP - 0x%04x\n", cpu->sp);
    printf("PC - 0x%04x\n", cpu->pc);
    printf("--flags--\n");
    printf("Sign - %d", cpu->cc.sign);
    printf("Zero - %d", cpu->cc.zero);
    printf("Aux - %d", cpu->cc.aux);
    printf("Parity - %d", cpu->cc.parity);
    printf("Carry - %d", cpu->cc.carry);
}

int execute(cpustate* state) {
    byte* opcode = state->memory + state->pc;
    int cycles;
    switch (*opcode) {
        case 0x00:  //NOP
            state->pc++;
            cycles = 4;
            break;
        case 0x01:  //LXI B, D16   store value D16 into register pair BC
            state->B = opcode[2];
            state->C = opcode[1];
            state->pc += 3;
            cycles = 10;
            break;
        case 0x02:  //STAX B  store the value in the accumulator at the memory address in BC
            state->memory[state->BC] = state->A;
            state->pc++;
            cycles = 7;
            break;
        case 0x03:  //INX B increase pair BC by 1
            state->BC++;
            state->pc++;
            cycles = 5;
            break;
        case 0x04:  //INR B increase B by 1
            state->B++;
            state->cc.zero = (state->B == 0);
            state->cc.sign = ((state->B & 0x80) == 0x80);  //0x80 = 0b10000000
            state->cc.parity = parity(state->B);
            state->pc++;
            cycles = 5;
            break;
        case 0x05:  //DCR B decrease B by 1
            state->B--;
            state->cc.zero = (state->B == 0);
            state->cc.sign = ((state->B & 0x80) == 0x80);  //0x80 = 0b10000000
            state->cc.parity = parity(state->B);
            state->pc++;
            cycles = 5;
            break;
        case 0x06:  //MVI B, D8  store d8 in B
            state->B = opcode[1];
            state->pc += 2;
            cycles = 7;
            break;
        case 0x07:  //RLC rotate A left
            state->cc.carry = (state->A & 0x80) == 0x80;
            state->A = (state->A << 1) | state->cc.carry;
            state->pc++;
            cycles = 4;
            break;
        case 0x08:  //NOP
            state->pc++;
            cycles = 4;
            break;
        case 0x09: {                                      //DAD B add contents of BC to HL, store in HL
            unsigned int result = state->BC + state->HL;  //higher precision to capture carry
            state->cc.carry = (result & 0xffff) > 0;
            state->HL = (twobyte)(result & 0x0000ffff);
            state->pc++;
            cycles = 10;
            break;
        }
        case 0x0a:  //LDAX B
            state->A = state->memory[state->BC];
            state->pc++;
            cycles = 7;
            break;
        case 0x0b:  //DCX B
            state->BC--;
            state->pc++;
            cycles = 5;
            break;
        case 0x0c:  //INR C
            state->C++;
            state->cc.zero = (state->C == 0);
            state->cc.sign = ((state->C & 0x80) == 0x80);  //0x80 = 0b10000000
            state->cc.parity = parity(state->C);
            state->pc++;
            cycles = 5;
            break;
        case 0x0d:  //DCR C  decrease C by one
            state->C--;
            state->cc.zero = (state->C == 0);
            state->cc.sign = ((state->C & 0x80) == 0x80);
            state->cc.parity = parity(state->C);
            state->pc++;
            cycles = 5;
            break;
        case 0x0e:  //MVI C, D8   copy D8 into C
            state->C = opcode[1];
            state->pc += 2;
            cycles = 7;
            break;
        case 0x0f:  //RRC rotate A right
            state->cc.carry = (state->A & 0x01);
            state->A = (state->A >> 1) | ((byte)state->cc.carry << 7);
            state->pc++;
            cycles = 4;
            break;
        case 0x10:  //NOP
            state->pc++;
            cycles = 4;
            break;
        case 0x11:  //LXI D, D16  load d16 into DC  byte 3->D byte 2->E
            state->D = opcode[2];
            state->E = opcode[1];
            state->pc += 3;
            cycles = 10;
            break;
        case 0x12:  //STAX D
            state->memory[state->DE] = state->A;
            state->pc++;
            cycles = 7;
            break;
        case 0x13:  //INX D
            state->DE++;
            state->pc++;
            cycles = 5;
            break;
        case 0x14:  //INR D
            state->D++;
            state->cc.zero = (state->D == 0);
            state->cc.sign = ((state->D & 0x80) == 0x80);  //0x80 = 0b10000000
            state->cc.parity = parity(state->D);
            state->pc++;
            cycles = 5;
            break;
        case 0x15:  //DCR D
            state->D--;
            state->cc.zero = (state->D == 0);
            state->cc.sign = ((state->D & 0x80) == 0x80);  //0x80 = 0b10000000
            state->cc.parity = parity(state->D);
            state->pc++;
            cycles = 5;
            break;
        case 0x16:  //MVI D, D8
            state->D = opcode[1];
            state->pc += 2;
            cycles = 7;
            break;
        case 0x17: {  //RAL
            byte t = state->cc.carry;
            state->cc.carry = (state->A & 0x80) == 0x80;
            state->A = (state->A << 1) | t;
            state->pc++;
            cycles = 4;
            break;
        }
        case 0x18:  //NOP
            state->pc++;
            cycles = 4;
            break;
        case 0x19: {                                      //DAD D
            unsigned int result = state->DE + state->HL;  //higher precision to capture carry
            state->cc.carry = (result & 0xffff) > 0;
            state->HL = (twobyte)(result & 0x0000ffff);
            state->pc++;
            cycles = 10;
            break;
        }
        case 0x1a:  //LDAX D load value at memory address in DE to Accumulator
            state->A = state->memory[state->DE];
            state->pc++;
            cycles = 7;
            break;
        case 0x1b:  //DCX D
            state->DE--;
            state->pc++;
            cycles = 5;
            break;
        case 0x1c:  //INR E
            state->E++;
            state->cc.zero = (state->E == 0);
            state->cc.sign = ((state->E & 0x80) == 0x80);  //0x80 = 0b10000000
            state->cc.parity = parity(state->E);
            state->pc++;
            cycles = 5;
            break;
        case 0x1d:  //DCR E
            state->E--;
            state->cc.zero = (state->E == 0);
            state->cc.sign = ((state->E & 0x80) == 0x80);  //0x80 = 0b10000000
            state->cc.parity = parity(state->E);
            state->pc++;
            cycles = 5;
            break;
        case 0x1e:  //MVI E, D8
            state->E = opcode[1];
            state->pc += 2;
            cycles = 7;
            break;
        case 0x1f: {  //RAR
            byte t = state->cc.carry;
            state->cc.carry = state->A & 0x01;
            state->A = (state->A >> 1) | (t << 7);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0x20:  //NOP
            state->pc++;
            cycles = 4;
            break;
        case 0x21:  //LXI H, D16  copy d16 to HL
            state->H = opcode[2];
            state->L = opcode[1];
            state->pc += 3;
            cycles = 10;
            break;
        case 0x22:  //SHLD adr
            state->memory[concat(opcode[2], opcode[1])] = state->L;
            state->memory[concat(opcode[2], opcode[1]) + 1] = state->H;
            state->pc += 3;
            cycles = 16;
            break;
        case 0x23:  //INX H
            state->HL++;
            state->pc++;
            cycles = 5;
            break;
        case 0x24:  //INR H
            state->H++;
            state->cc.zero = (state->H == 0);
            state->cc.sign = ((state->H & 0x80) == 0x80);  //0x80 = 0b10000000
            state->cc.parity = parity(state->H);
            state->pc++;
            cycles = 5;
            break;
        case 0x25:  //DCR H
            state->H--;
            state->cc.zero = (state->H == 0);
            state->cc.sign = ((state->H & 0x80) == 0x80);  //0x80 = 0b10000000
            state->cc.parity = parity(state->H);
            state->pc++;
            cycles = 5;
            break;
        case 0x26:  //MVI H,D8
            state->H = opcode[1];
            state->pc += 2;
            cycles = 7;
            break;
        case 0x27:  //DAA - not implemented
            state->pc++;
            cycles = -4;
            break;
        case 0x28:  //NOP
            state->pc++;
            cycles = 4;
            break;
        case 0x29: {                                      //DAD H
            unsigned int result = state->HL + state->HL;  //higher precision to capture carry
            state->cc.carry = (result & 0xffff) > 0;
            state->HL = (twobyte)(result & 0x0000ffff);
            state->pc++;
            cycles = 10;
            break;
        }
        case 0x2a:  //LHLD adr
            state->L = state->memory[concat(opcode[2], opcode[1])];
            state->H = state->memory[concat(opcode[2], opcode[1]) + 1];
            state->pc += 3;
            cycles = 16;
            break;
        case 0x2b:  //DCX H
            state->HL--;
            state->pc++;
            cycles = 5;
            break;
        case 0x2c:  //INR L
            state->L++;
            state->cc.zero = (state->L == 0);
            state->cc.sign = ((state->L & 0x80) == 0x80);  //0x80 = 0b10000000
            state->cc.parity = parity(state->L);
            state->pc++;
            cycles = 5;
            break;
        case 0x2d:  //DCR L
            state->L--;
            state->cc.zero = (state->L == 0);
            state->cc.sign = ((state->L & 0x80) == 0x80);  //0x80 = 0b10000000
            state->cc.parity = parity(state->L);
            state->pc++;
            cycles = 5;
            break;
        case 0x2e:  //MVI L, D8
            state->L = opcode[1];
            state->pc += 2;
            cycles = 7;
            break;
        case 0x2f:  //CMA
            state->A = ~state->A;
            state->pc++;
            cycles = 4;
            break;
        case 0x30:  //NOP
            state->pc++;
            cycles = 4;
            break;
        case 0x31:  //LXI SP, D16
            state->sp = concat(opcode[2], opcode[1]);
            state->pc += 3;
            cycles = 10;
            break;
        case 0x32:  //STA adr
            state->memory[concat(opcode[2], opcode[1])] = state->A;
            state->pc += 3;
            cycles = 13;
            break;
        case 0x33:  //INX SP
            state->sp++;
            state->pc++;
            cycles = 5;
            break;
        case 0x34:  //INR M
            state->HL++;
            state->cc.zero = (state->HL == 0);
            state->cc.sign = ((state->HL & 0x8000) == 0x8000);  //0x80 = 0b10000000
            state->cc.parity = parity(state->HL);
            state->pc++;
            cycles = 10;
            break;
        case 0x35:  //DCR M
            state->HL--;
            state->cc.zero = (state->HL == 0);
            state->cc.sign = ((state->HL & 0x8000) == 0x8000);  //0x80 = 0b10000000
            state->cc.parity = parity(state->HL);
            state->pc++;
            cycles = 10;
            break;
        case 0x36:  //MVI M, D8
            state->memory[state->HL] = opcode[1];
            state->pc += 2;
            cycles = 10;
            break;
        case 0x37:  //STC
            state->cc.carry = 1;
            state->pc++;
            cycles = 4;
            break;
        case 0x38:  //NOP
            state->pc++;
            cycles = 4;
            break;
        case 0x39: {                                      //DAD SP
            unsigned int result = state->sp + state->HL;  //higher precision to capture carry
            state->cc.carry = (result & 0xffff) > 0;
            state->HL = (twobyte)(result & 0x0000ffff);
            state->pc++;
            cycles = 10;
            break;
        }
        case 0x3a:  //LDA adr
            state->A = state->memory[concat(opcode[2], opcode[1])];
            state->pc += 3;
            cycles = 13;
            break;
        case 0x3b:  //DCX SP
            state->sp--;
            state->pc++;
            cycles = 5;
            break;
        case 0x3c:  //INR A
            state->A++;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);  //0x80 = 0b10000000
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 5;
            break;
        case 0x3d:  //DCR A
            state->A--;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);  //0x80 = 0b10000000
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 5;
            break;
        case 0x3e:  //MVI A, D8
            state->A = opcode[1];
            state->pc += 2;
            break;
        case 0x3f:  //CMC
            state->cc.carry = ~state->cc.carry;
            state->pc++;
            cycles = 4;
            break;
        case 0x40:  //MOV Bs
            state->B = state->B;
            state->pc++;
            cycles = 5;
            break;
        case 0x41:
            state->B = state->C;
            state->pc++;
            cycles = 5;
            break;
        case 0x42:
            state->B = state->D;
            state->pc++;
            cycles = 5;
            break;
        case 0x43:
            state->B = state->E;
            state->pc++;
            cycles = 5;
            break;
        case 0x44:
            state->B = state->H;
            state->pc++;
            cycles = 5;
            break;
        case 0x45:
            state->B = state->L;
            state->pc++;
            cycles = 5;
            break;
        case 0x46:
            state->B = state->memory[state->HL];
            state->pc++;
            cycles = 7;
            break;
        case 0x47:
            state->B = state->A;
            state->pc++;
            cycles = 5;
            break;
        case 0x48:  //MOV Cs
            state->C = state->B;
            state->pc++;
            cycles = 5;
            break;
        case 0x49:
            state->C = state->C;
            state->pc++;
            cycles = 5;
            break;
        case 0x4a:
            state->C = state->D;
            state->pc++;
            cycles = 5;
            break;
        case 0x4b:
            state->C = state->E;
            state->pc++;
            cycles = 5;
            break;
        case 0x4c:
            state->C = state->H;
            state->pc++;
            cycles = 5;
            break;
        case 0x4d:
            state->C = state->L;
            state->pc++;
            cycles = 5;
            break;
        case 0x4e:
            state->C = state->memory[state->HL];
            state->pc++;
            cycles = 7;
            break;
        case 0x4f:
            state->C = state->A;
            state->pc++;
            cycles = 5;
            break;
        case 0x50:  //MOV Ds
            state->D = state->B;
            state->pc++;
            cycles = 5;
            break;
        case 0x51:
            state->D = state->C;
            state->pc++;
            cycles = 5;
            break;
        case 0x52:
            state->D = state->D;
            state->pc++;
            cycles = 5;
            break;
        case 0x53:
            state->D = state->E;
            state->pc++;
            cycles = 5;
            break;
        case 0x54:
            state->D = state->H;
            state->pc++;
            cycles = 5;
            break;
        case 0x55:
            state->D = state->L;
            state->pc++;
            cycles = 5;
            break;
        case 0x56:  //MOV D, M
            state->D = state->memory[state->HL];
            state->pc++;
            cycles = 7;
            break;
        case 0x57:
            state->D = state->A;
            state->pc++;
            cycles = 5;
            break;
        case 0x58:  //MOV Es
            state->E = state->B;
            state->pc++;
            cycles = 5;
            break;
        case 0x59:
            state->E = state->C;
            state->pc++;
            cycles = 5;
            break;
        case 0x5a:
            state->E = state->D;
            state->pc++;
            cycles = 5;
            break;
        case 0x5b:
            state->E = state->E;
            state->pc++;
            cycles = 5;
            break;
        case 0x5c:
            state->E = state->H;
            state->pc++;
            cycles = 5;
            break;
        case 0x5d:
            state->E = state->L;
            state->pc++;
            cycles = 5;
            break;
        case 0x5e:  //MOV E, M
            state->E = state->memory[state->HL];
            state->pc++;
            cycles = 7;
            break;
        case 0x5f:
            state->E = state->A;
            state->pc++;
            cycles = 5;
            break;
        case 0x60:  //MOV Hs
            state->H = state->B;
            state->pc++;
            cycles = 5;
            break;
        case 0x61:
            state->H = state->C;
            state->pc++;
            cycles = 5;
            break;
        case 0x62:
            state->H = state->D;
            state->pc++;
            cycles = 5;
            break;
        case 0x63:
            state->H = state->E;
            state->pc++;
            cycles = 5;
            break;
        case 0x64:
            state->H = state->H;
            state->pc++;
            cycles = 5;
            break;
        case 0x65:
            state->H = state->L;
            state->pc++;
            cycles = 5;
            break;
        case 0x66:  //MOV H, M
            state->H = state->memory[state->HL];
            state->pc++;
            cycles = 7;
            break;
        case 0x67:
            state->H = state->A;
            state->pc++;
            cycles = 5;
            break;
        case 0x68:  //MOV Ls
            state->L = state->B;
            state->pc++;
            cycles = 5;
            break;
        case 0x69:
            state->L = state->C;
            state->pc++;
            cycles = 5;
            break;
        case 0x6a:
            state->L = state->D;
            state->pc++;
            cycles = 5;
            break;
        case 0x6b:
            state->L = state->E;
            state->pc++;
            cycles = 5;
            break;
        case 0x6c:
            state->L = state->H;
            state->pc++;
            cycles = 5;
            break;
        case 0x6d:
            state->L = state->L;
            state->pc++;
            cycles = 5;
            break;
        case 0x6e:
            state->L = state->memory[state->HL];
            state->pc++;
            cycles = 7;
            break;
        case 0x6f:  //MOV L,A
            state->L = state->A;
            state->pc++;
            cycles = 5;
            break;
        case 0x70:  //MOV Ms
            state->memory[state->HL] = state->B;
            state->pc++;
            cycles = 7;
            break;
        case 0x71:
            state->memory[state->HL] = state->C;
            state->pc++;
            cycles = 7;
            break;
        case 0x72:
            state->memory[state->HL] = state->D;
            state->pc++;
            cycles = 7;
            break;
        case 0x73:
            state->memory[state->HL] = state->E;
            state->pc++;
            cycles = 7;
            break;
        case 0x74:
            state->memory[state->HL] = state->H;
            state->pc++;
            cycles = 7;
            break;
        case 0x75:
            state->memory[state->HL] = state->L;
            state->pc++;
            cycles = 7;
            break;
        case 0x76:  //HLT
            state->pc++;
            cycles = -7;
            break;
        case 0x77:  //MOV M,A
            state->memory[state->HL] = state->A;
            state->pc++;
            cycles = 7;
            break;
        case 0x78:  //MOV As
            state->A = state->B;
            state->pc++;
            cycles = 5;
            break;
        case 0x79:
            state->A = state->C;
            state->pc++;
            cycles = 5;
            break;
        case 0x7a:  //MOV A,D
            state->A = state->D;
            state->pc++;
            cycles = 5;
            break;
        case 0x7b:  //MOV A,E
            state->A = state->E;
            state->pc++;
            cycles = 5;
            break;
        case 0x7c:  //MOV A,H
            state->A = state->H;
            state->pc++;
            cycles = 5;
            break;
        case 0x7d:
            state->A = state->L;
            state->pc++;
            cycles = 5;
            break;
        case 0x7e:  //MOV A,M
            state->A = state->memory[state->HL];
            state->pc++;
            cycles = 7;
            break;
        case 0x7f:
            state->A = state->A;
            state->pc++;
            cycles = 5;
            break;
        case 0x80: {  //ADD B
            twobyte ans = state->A + state->B;
            state->cc.carry = (ans & 0xff00) > 0;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0x81: {  //ADD C
            twobyte ans = state->A + state->C;
            state->cc.carry = (ans & 0xff00) > 0;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0x82: {  //ADD D
            twobyte ans = state->A + state->D;
            state->cc.carry = (ans & 0xff00) > 0;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0x83: {  //ADD E
            twobyte ans = state->A + state->E;
            state->cc.carry = (ans & 0xff00) > 0;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0x84: {  //ADD H
            twobyte ans = state->A + state->H;
            state->cc.carry = (ans & 0xff00) > 0;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0x85: {  //ADD L
            twobyte ans = state->A + state->L;
            state->cc.carry = (ans & 0xff00) > 0;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0x86: {  //ADD M
            twobyte ans = state->A + state->memory[state->HL];
            state->cc.carry = (ans & 0xff00) > 0;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 7;
            break;
        }
        case 0x87: {  //ADD A
            twobyte ans = state->A + state->A;
            state->cc.carry = (ans & 0xff00) > 0;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0x88: {  //ADC B
            twobyte ans = state->A + state->B + state->cc.carry;
            state->cc.carry = (ans & 0xff00) > 0;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0x89: {  //ADC C
            twobyte ans = state->A + state->C + state->cc.carry;
            state->cc.carry = (ans & 0xff00) > 0;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0x8a: {  //ADC D
            twobyte ans = state->A + state->D + state->cc.carry;
            state->cc.carry = (ans & 0xff00) > 0;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0x8b: {  //ADC E
            twobyte ans = state->A + state->E + state->cc.carry;
            state->cc.carry = (ans & 0xff00) > 0;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0x8c: {  //ADC H
            twobyte ans = state->A + state->H + state->cc.carry;
            state->cc.carry = (ans & 0xff00) > 0;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0x8d: {  //ADC L
            twobyte ans = state->A + state->L + state->cc.carry;
            state->cc.carry = (ans & 0xff00) > 0;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0x8e: {  //ADC M
            twobyte ans = state->A + state->memory[state->HL] + state->cc.carry;
            state->cc.carry = (ans & 0xff00) > 0;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 7;
            break;
        }
        case 0x8f: {  //ADC A
            twobyte ans = state->A + state->A + state->cc.carry;
            state->cc.carry = (ans & 0xff00) > 0;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0x90: {  //SUB B
            twobyte ans = state->A - state->B;
            state->cc.carry = (ans & 0x0100) == 0x0100;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0x91: {  //SUB C
            twobyte ans = state->A - state->C;
            state->cc.carry = (ans & 0x0100) == 0x0100;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0x92: {  //SUB D
            twobyte ans = state->A - state->D;
            state->cc.carry = (ans & 0x0100) == 0x0100;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0x93: {  //SUB E
            twobyte ans = state->A - state->E;
            state->cc.carry = (ans & 0x0100) == 0x0100;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0x94: {  //SUB H
            twobyte ans = state->A - state->H;
            state->cc.carry = (ans & 0x0100) == 0x0100;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0x95: {  //SUB L
            twobyte ans = state->A - state->L;
            state->cc.carry = (ans & 0x0100) == 0x0100;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0x96: {  //SUB M
            twobyte ans = state->A - state->memory[state->HL];
            state->cc.carry = (ans & 0x0100) == 0x0100;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 7;
            break;
        }
        case 0x97: {  //SUB A
            twobyte ans = state->A - state->A;
            state->cc.carry = (ans & 0x0100) == 0x0100;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0x98: {  //SBB B
            twobyte ans = state->A - (state->B + state->cc.carry);
            state->cc.carry = (ans & 0x0100) == 0x0100;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0x99: {  //SBB C
            twobyte ans = state->A - (state->C + state->cc.carry);
            state->cc.carry = (ans & 0x0100) == 0x0100;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0x9a: {  //SBB D
            twobyte ans = state->A - (state->D + state->cc.carry);
            state->cc.carry = (ans & 0x0100) == 0x0100;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0x9b: {  //SBB E
            twobyte ans = state->A - (state->E + state->cc.carry);
            state->cc.carry = (ans & 0x0100) == 0x0100;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0x9c: {  //SBB H
            twobyte ans = state->A - (state->H + state->cc.carry);
            state->cc.carry = (ans & 0x0100) == 0x0100;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0x9d: {  //SBB L
            twobyte ans = state->A - (state->L + state->cc.carry);
            state->cc.carry = (ans & 0x0100) == 0x0100;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0x9e: {  //SBB M
            twobyte ans = state->A - (state->memory[state->HL] + state->cc.carry);
            state->cc.carry = (ans & 0x0100) == 0x0100;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 7;
            break;
        }
        case 0x9f: {  //SBB A
            twobyte ans = state->A - (state->A + state->cc.carry);
            state->cc.carry = (ans & 0x0100) == 0x0100;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0xa0:  //ANA B
            state->A = state->A & state->B;
            state->cc.carry = 0;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        case 0xa1:  //ANA C
            state->A = state->A & state->C;
            state->cc.carry = 0;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        case 0xa2:
            state->A = state->A & state->D;
            state->cc.carry = 0;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        case 0xa3:
            state->A = state->A & state->E;
            state->cc.carry = 0;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        case 0xa4:
            state->A = state->A & state->H;
            state->cc.carry = 0;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        case 0xa5:
            state->A = state->A & state->L;
            state->cc.carry = 0;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        case 0xa6:  //ANA M
            state->A = state->A & state->memory[state->HL];
            state->cc.carry = 0;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 7;
            break;
        case 0xa7:  //ANA A
            state->A = state->A & state->A;
            state->cc.carry = 0;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        case 0xa8:  //XRA B
            state->A = state->A ^ state->B;
            state->cc.carry = 0;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        case 0xa9:
            state->A = state->A ^ state->C;
            state->cc.carry = 0;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        case 0xaa:
            state->A = state->A ^ state->D;
            state->cc.carry = 0;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        case 0xab:
            state->A = state->A ^ state->E;
            state->cc.carry = 0;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        case 0xac:
            state->A = state->A ^ state->H;
            state->cc.carry = 0;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        case 0xad:
            state->A = state->A ^ state->L;
            state->cc.carry = 0;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        case 0xae:
            state->A = state->A ^ state->memory[state->HL];
            state->cc.carry = 0;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 7;
            break;
        case 0xaf:  //XRA A
            state->A = state->A ^ state->A;
            state->cc.carry = 0;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        case 0xb0:  //ORA B
            state->A = state->A | state->B;
            state->cc.carry = 0;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        case 0xb1:
            state->A = state->A | state->C;
            state->cc.carry = 0;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        case 0xb2:
            state->A = state->A | state->D;
            state->cc.carry = 0;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        case 0xb3:
            state->A = state->A | state->E;
            state->cc.carry = 0;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        case 0xb4:
            state->A = state->A | state->H;
            state->cc.carry = 0;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        case 0xb5:
            state->A = state->A | state->L;
            state->cc.carry = 0;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            break;
            cycles = 4;
        case 0xb6:
            state->A = state->A | state->memory[state->HL];
            state->cc.carry = 0;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 7;
            break;
        case 0xb7:
            state->A = state->A | state->A;
            state->cc.carry = 0;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc++;
            cycles = 4;
            break;
        case 0xb8: {  //CMP B
            twobyte ans = state->A - state->B;
            state->cc.carry = (ans & 0x0100) == 0x0100;
            if ((state->A & 0x80) != (state->B & 0x80)) {
                state->cc.carry = !state->cc.carry;
            };
            if (state->B == 0) {
                state->cc.carry = 0;
            }
            ans = ans & 0xff;
            state->cc.zero = (ans == 0);
            state->cc.sign = ((ans & 0x80) == 0x80);
            state->cc.parity = parity(ans);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0xb9: {  //CMP C
            twobyte ans = state->A - state->C;
            state->cc.carry = (ans & 0x0100) == 0x0100;
            if ((state->A & 0x80) != (state->C & 0x80)) {
                state->cc.carry = !state->cc.carry;
            };
            if (state->C == 0) {
                state->cc.carry = 0;
            }
            ans = ans & 0xff;
            state->cc.zero = (ans == 0);
            state->cc.sign = ((ans & 0x80) == 0x80);
            state->cc.parity = parity(ans);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0xba: {  //CMP D
            twobyte ans = state->A - state->D;
            state->cc.carry = (ans & 0x0100) == 0x0100;
            if ((state->A & 0x80) != (state->D & 0x80)) {
                state->cc.carry = !state->cc.carry;
            };
            if (state->D == 0) {
                state->cc.carry = 0;
            }
            ans = ans & 0xff;
            state->cc.zero = (ans == 0);
            state->cc.sign = ((ans & 0x80) == 0x80);
            state->cc.parity = parity(ans);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0xbb: {  //CMP E
            twobyte ans = state->A - state->E;
            state->cc.carry = (ans & 0x0100) == 0x0100;
            if ((state->A & 0x80) != (state->E & 0x80)) {
                state->cc.carry = !state->cc.carry;
            };
            if (state->E == 0) {
                state->cc.carry = 0;
            }
            ans = ans & 0xff;
            state->cc.zero = (ans == 0);
            state->cc.sign = ((ans & 0x80) == 0x80);
            state->cc.parity = parity(ans);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0xbc: {  //CMP H
            twobyte ans = state->A - state->H;
            state->cc.carry = (ans & 0x0100) == 0x0100;
            if ((state->A & 0x80) != (state->H & 0x80)) {
                state->cc.carry = !state->cc.carry;
            };
            if (state->H == 0) {
                state->cc.carry = 0;
            }
            ans = ans & 0xff;
            state->cc.zero = (ans == 0);
            state->cc.sign = ((ans & 0x80) == 0x80);
            state->cc.parity = parity(ans);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0xbd: {  //CMP L
            twobyte ans = state->A - state->L;
            state->cc.carry = (ans & 0x0100) == 0x0100;
            if ((state->A & 0x80) != (state->L & 0x80)) {
                state->cc.carry = !state->cc.carry;
            };
            if (state->L == 0) {
                state->cc.carry = 0;
            }
            ans = ans & 0xff;
            state->cc.zero = (ans == 0);
            state->cc.sign = ((ans & 0x80) == 0x80);
            state->cc.parity = parity(ans);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0xbe: {  //CMP M
            twobyte ans = state->A - state->memory[state->HL];
            state->cc.carry = (ans & 0x0100) == 0x0100;
            if ((state->A & 0x80) != (state->memory[state->HL] & 0x80)) {
                state->cc.carry = !state->cc.carry;
            };
            if (state->memory[state->HL] == 0) {
                state->cc.carry = 0;
            }
            ans = ans & 0xff;
            state->cc.zero = (ans == 0);
            state->cc.sign = ((ans & 0x80) == 0x80);
            state->cc.parity = parity(ans);
            state->pc++;
            cycles = 7;
            break;
        }
        case 0xbf: {  //CMP A
            twobyte ans = state->A - state->A;
            state->cc.carry = (ans & 0x0100) == 0x0100;
            if ((state->A & 0x80) != (state->A & 0x80)) {
                state->cc.carry = !state->cc.carry;
            };
            if (state->A == 0) {
                state->cc.carry = 0;
            }
            ans = ans & 0xff;
            state->cc.zero = (ans == 0);
            state->cc.sign = ((ans & 0x80) == 0x80);
            state->cc.parity = parity(ans);
            state->pc++;
            cycles = 4;
            break;
        }
        case 0xc0:  //RNZ
            if (state->cc.zero) {
                state->pc++;
                cycles = 5;
            } else {
                state->pc = concat(state->memory[state->sp + 1], state->memory[state->sp]);
                state->sp += 2;
                cycles = 11;
            }
            break;
        case 0xc1:  //POP B
            state->B = state->memory[state->sp + 1];
            state->C = state->memory[state->sp];
            state->sp += 2;
            state->pc++;
            cycles = 10;
            break;
        case 0xc2:  //JNZ adr
            if (state->cc.zero) {
                state->pc += 3;
            } else {
                state->pc = concat(opcode[2], opcode[1]);
            }
            cycles = 10;
            break;
        case 0xc3:  //JMP adr
            state->pc = concat(opcode[2], opcode[1]);
            cycles = 10;
            break;
        case 0xc4:  //CNZ adr
            if (state->cc.zero) {
                state->pc += 3;
                cycles = 11;
            } else {
                state->memory[state->sp - 1] = (byte)((state->pc + 3) >> 8);
                state->memory[state->sp - 2] = (byte)(state->pc + 3);
                state->sp -= 2;
                state->pc = concat(opcode[2], opcode[1]);
                cycles = 17;
            }
            break;
        case 0xc5:  //PUSH B
            state->memory[state->sp - 1] = state->B;
            state->memory[state->sp - 2] = state->C;
            state->sp -= 2;
            state->pc++;
            cycles = 11;
            break;
        case 0xc6: {  //ADI D8
            twobyte answer = state->A + opcode[1];
            state->cc.carry = (answer & 0xff00) > 0;
            state->A = (byte)(answer & 0x00ff);
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc += 2;
            cycles = 7;
            break;
        }
        case 0xc7:  //RST 0
            state->memory[state->sp - 1] = (byte)((state->pc + 3) >> 8);
            state->memory[state->sp - 2] = (byte)(state->pc + 3);
            state->sp -= 2;
            state->pc = 0;
            cycles = 11;
            break;
        case 0xc8:  //RZ
            if (state->cc.zero) {
                state->pc = concat(state->memory[state->sp + 1], state->memory[state->sp]);
                state->sp += 2;
                cycles = 5;
            } else {
                state->pc++;
                cycles = 11;
            }
            break;
        case 0xc9:  //RET
            state->pc = concat(state->memory[state->sp + 1], state->memory[state->sp]);
            state->sp += 2;
            cycles = 10;
            break;
        case 0xca:  //JZ ADR
            if (state->cc.zero) {
                state->pc = concat(opcode[2], opcode[1]);
            } else {
                state->pc += 3;
            }
            cycles = 10;
            break;
        case 0xcb:  //NOP
            state->pc++;
            cycles = 4;
            break;
        case 0xcc:  //CZ adr
            if (state->cc.zero) {
                state->memory[state->sp - 1] = (byte)((state->pc + 3) >> 8);
                state->memory[state->sp - 2] = (byte)(state->pc + 3);
                state->sp -= 2;
                state->pc = concat(opcode[2], opcode[1]);
                cycles = 17;
            } else {
                state->pc += 3;
                cycles = 11;
            }
            break;
        case 0xcd:  //CALL adr
            state->memory[state->sp - 1] = (byte)((state->pc + 3) >> 8);
            state->memory[state->sp - 2] = (byte)(state->pc + 3);
            state->sp -= 2;
            state->pc = concat(opcode[2], opcode[1]);
            cycles = 17;
            break;
        case 0xce: {  //ACI D8
            twobyte answer = state->A + opcode[1] + state->cc.carry;
            state->cc.carry = (answer & 0xff00) > 0;
            state->A = (byte)(answer & 0x00ff);
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc += 2;
            cycles = 7;
            break;
        }
        case 0xcf:  //RST 1
            state->memory[state->sp - 1] = (byte)((state->pc + 3) >> 8);
            state->memory[state->sp - 2] = (byte)(state->pc + 3);
            state->sp -= 2;
            state->pc = 0x08;
            cycles = 11;
            break;
        case 0xd0:  //RNC
            if (state->cc.carry) {
                state->pc++;
                cycles = 5;
            } else {
                state->pc = concat(state->memory[state->sp + 1], state->memory[state->sp]);
                state->sp += 2;
                cycles = 11;
            }
            break;
        case 0xd1:  //POP D
            state->D = state->memory[state->sp + 1];
            state->E = state->memory[state->sp];
            state->sp += 2;
            state->pc++;
            cycles = 10;
            break;
        case 0xd2:  //JNC adr
            if (state->cc.carry) {
                state->pc += 3;
            } else {
                state->pc = concat(opcode[2], opcode[1]);
            }
            cycles = 10;
            break;
        case 0xd3:  //OUT d8
            state->outspace[opcode[1]] = state->A;
            state->pc += 2;
            cycles = 10;
            break;
        case 0xd4:  //CNC adr
            if (state->cc.carry) {
                state->pc += 3;
                cycles = 11;
            } else {
                state->memory[state->sp - 1] = (byte)((state->pc + 3) >> 8);
                state->memory[state->sp - 2] = (byte)(state->pc + 3);
                state->sp -= 2;
                state->pc = concat(opcode[2], opcode[1]);
                cycles = 17;
            }
            break;
        case 0xd5:  //PUSH D
            state->memory[state->sp - 1] = state->D;
            state->memory[state->sp - 2] = state->E;
            state->sp -= 2;
            state->pc++;
            cycles = 11;
            break;
        case 0xd6: {  //SUI D8
            twobyte ans = state->A - opcode[1];
            state->cc.carry = (ans & 0x0100) == 0x0100;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc += 2;
            cycles = 7;
            break;
        }
        case 0xd7:  //RST 2
            state->memory[state->sp - 1] = (byte)((state->pc + 3) >> 8);
            state->memory[state->sp - 2] = (byte)(state->pc + 3);
            state->sp -= 2;
            state->pc = 0x10;
            cycles = 11;
            break;
        case 0xd8:  //RC
            if (state->cc.carry) {
                state->pc = concat(state->memory[state->sp + 1], state->memory[state->sp]);
                state->sp += 2;
                cycles = 11;
            } else {
                state->pc++;
                cycles = 5;
            }
            break;
        case 0xd9:  //NOP
            state->pc++;
            cycles = 4;
            break;
        case 0xda:  //JC adr
            if (state->cc.carry) {
                state->pc = concat(opcode[2], opcode[1]);
            } else {
                state->pc += 3;
            }
            cycles = 10;
            break;
        case 0xdb:  //IN D8
            state->A = state->inspace[opcode[1]];
            state->pc += 2;
            cycles = 10;
            break;
        case 0xdc:  //CC adr
            if (state->cc.carry) {
                state->memory[state->sp - 1] = (byte)((state->pc + 3) >> 8);
                state->memory[state->sp - 2] = (byte)(state->pc + 3);
                state->sp -= 2;
                state->pc = concat(opcode[2], opcode[1]);
                cycles = 17;
            } else {
                state->pc += 3;
                cycles = 11;
            }
            break;
        case 0xdd:  //NOP
            state->pc++;
            cycles = 4;
            break;
        case 0xde: {  //SBI D8
            twobyte ans = state->A - (opcode[1] + state->cc.carry);
            state->cc.carry = (ans & 0x0100) == 0x0100;
            state->A = ans;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc += 2;
            cycles = 7;
            break;
        }
        case 0xdf:  //RST 3
            state->memory[state->sp - 1] = (byte)((state->pc + 3) >> 8);
            state->memory[state->sp - 2] = (byte)(state->pc + 3);
            state->sp -= 2;
            state->pc = 0x18;
            cycles = 11;
            break;
        case 0xe0:  //RPO
            if (state->cc.parity) {
                state->pc++;
                cycles = 5;
            } else {
                state->pc = concat(state->memory[state->sp + 1], state->memory[state->sp]);
                state->sp += 2;
                cycles = 11;
            }
            break;
        case 0xe1:  //POP H
            state->H = state->memory[state->sp + 1];
            state->L = state->memory[state->sp];
            state->sp += 2;
            state->pc++;
            cycles = 10;
            break;
        case 0xe2:  //JPO adr
            if (state->cc.parity) {
                state->pc += 3;
            } else {
                state->pc = concat(opcode[2], opcode[1]);
            }
            cycles = 10;
            break;
        case 0xe3: {  //XTHL
            byte th = state->H;
            byte tl = state->L;
            state->L = state->memory[state->sp];
            state->H = state->memory[state->sp + 1];
            state->memory[state->sp] = tl;
            state->memory[state->sp + 1] = th;
            state->pc++;
            cycles = 18;
            break;
        }
        case 0xe4:  //CPO adr
            if (state->cc.parity) {
                state->pc += 3;
                cycles = 11;
            } else {
                state->memory[state->sp - 1] = (byte)((state->pc + 3) >> 8);
                state->memory[state->sp - 2] = (byte)(state->pc + 3);
                state->sp -= 2;
                state->pc = concat(opcode[2], opcode[1]);
                cycles = 17;
            }
            break;
        case 0xe5:  //PUSH H
            state->memory[state->sp - 1] = state->H;
            state->memory[state->sp - 2] = state->L;
            state->sp -= 2;
            state->pc++;
            cycles = 11;
            break;
        case 0xe6:  //ANI D8
            state->A = state->A & opcode[1];
            state->cc.carry = 0;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc += 2;
            cycles = 7;
            break;
        case 0xe7:  //RST 4
            state->memory[state->sp - 1] = (byte)((state->pc + 3) >> 8);
            state->memory[state->sp - 2] = (byte)(state->pc + 3);
            state->sp -= 2;
            state->pc = 0x20;
            cycles = 11;
            break;
        case 0xe8:  //RPE
            if (state->cc.parity) {
                state->pc = concat(state->memory[state->sp + 1], state->memory[state->sp]);
                state->sp += 2;
                cycles = 5;
            } else {
                state->pc++;
                cycles = 11;
            }
            break;
        case 0xe9:  //PCHL
            state->pc = state->HL;
            cycles = 5;
            break;
        case 0xea:  //JPE
            if (state->cc.parity) {
                state->pc = concat(opcode[2], opcode[1]);
            } else {
                state->pc += 3;
            }
            cycles = 10;
            break;
        case 0xeb: {  //XCHG
            twobyte t;
            t = state->HL;
            state->HL = state->DE;
            state->DE = t;
            state->pc++;
            cycles = 4;
            break;
        }
        case 0xec:  //CPE adr
            if (state->cc.parity) {
                state->memory[state->sp - 1] = (byte)((state->pc + 3) >> 8);
                state->memory[state->sp - 2] = (byte)(state->pc + 3);
                state->sp -= 2;
                state->pc = concat(opcode[2], opcode[1]);
                cycles = 17;
            } else {
                state->pc += 3;
                cycles = 11;
            }
            break;
        case 0xed:  //NOP
            state->pc++;
            cycles = 4;
            break;
        case 0xee:  //XRI D8
            state->A = state->A ^ opcode[1];
            state->cc.carry = 0;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc += 2;
            cycles = 7;
            break;
        case 0xef:  //RST 5
            state->memory[state->sp - 1] = (byte)((state->pc + 3) >> 8);
            state->memory[state->sp - 2] = (byte)(state->pc + 3);
            state->sp -= 2;
            state->pc = 0x28;
            cycles = 11;
            break;
        case 0xf0:  //RP
            if (state->cc.sign) {
                state->pc++;
                cycles = 5;
            } else {
                state->pc = concat(state->memory[state->sp + 1], state->memory[state->sp]);
                state->sp += 2;
                cycles = 11;
            }
            break;
        case 0xf1: {  //POP PSW
            state->A = state->memory[state->sp + 1];
            byte ccs = state->memory[state->sp];
            state->cc.sign = (ccs & 0b10000000) == 0b10000000;
            state->cc.zero = (ccs & 0b01000000) == 0b01000000;
            state->cc.aux = (ccs & 0b00010000) == 0b00010000;
            state->cc.parity = (ccs & 0b00000100) == 0b00000100;
            state->cc.carry = (ccs & 0b00000001) == 0b00000001;
            state->sp += 2;
            state->pc += 1;
            cycles = 10;
            break;
        }
        case 0xf2:  //JP adr
            if (state->cc.sign) {
                state->pc += 3;
            } else {
                state->pc = concat(opcode[2], opcode[1]);
            }
            cycles = 10;
            break;
        case 0xf3:  //DI
            state->int_enable = 0;
            state->pc++;
            cycles = 4;
            break;
        case 0xf4:  //CP adr
            if (state->cc.sign) {
                state->pc += 3;
                cycles = 11;
            } else {
                state->memory[state->sp - 1] = (byte)((state->pc + 3) >> 8);
                state->memory[state->sp - 2] = (byte)(state->pc + 3);
                state->sp -= 2;
                state->pc = concat(opcode[2], opcode[1]);
                cycles = 17;
            }
            break;
        case 0xf5:  //PUSH PSW
            state->memory[state->sp - 1] = state->A;
            state->memory[state->sp - 2] = (state->cc.sign << 7 & 0b10000000) |
                                           (state->cc.zero << 6 & 0b01000000) |
                                           (state->cc.aux << 4 & 0b00010000) |
                                           (state->cc.parity << 2 & 0b00000100) |
                                           (state->cc.carry & 0b00000001) & 0b00000010;  //condition byte
            state->sp -= 2;
            state->pc++;
            cycles = 11;
            break;
        case 0xf6:  //ORI D8
            state->A = state->A | opcode[1];
            state->cc.carry = 0;
            state->cc.zero = (state->A == 0);
            state->cc.sign = ((state->A & 0x80) == 0x80);
            state->cc.parity = parity(state->A);
            state->pc += 2;
            cycles = 7;
            break;
        case 0xf7:  //RST 6
            state->memory[state->sp - 1] = (byte)((state->pc + 3) >> 8);
            state->memory[state->sp - 2] = (byte)(state->pc + 3);
            state->sp -= 2;
            state->pc = 0x30;
            cycles = 11;
            break;
        case 0xf8:  //RM
            if (state->cc.sign) {
                state->pc = concat(state->memory[state->sp + 1], state->memory[state->sp]);
                state->sp += 2;
                cycles = 11;
            } else {
                state->pc++;
                cycles = 5;
            }
            break;
        case 0xf9:  //SPHL
            state->sp = state->HL;
            state->pc++;
            cycles = 5;
            break;
        case 0xfa:  //JM adr
            if (state->cc.sign) {
                state->pc = concat(opcode[2], opcode[1]);
            } else {
                state->pc += 3;
            }
            cycles = 10;
            break;
        case 0xfb:  //EI enable interrupts
            state->int_enable = 1;
            state->pc++;
            cycles = 4;
            break;
        case 0xfc:  //CM adr
            if (state->cc.sign) {
                state->memory[state->sp - 1] = (byte)((state->pc + 3) >> 8);
                state->memory[state->sp - 2] = (byte)(state->pc + 3);
                state->sp -= 2;
                state->pc = concat(opcode[2], opcode[1]);
                cycles = 17;
            } else {
                state->pc += 3;
                cycles = 11;
            }
            break;
        case 0xfd:  //NOP
            state->pc++;
            cycles = 4;
            break;
        case 0xfe: {  //CPI D8
            twobyte ans = state->A - opcode[1];
            state->cc.carry = (ans & 0x0100) == 0x0100;
            if (((state->A & 0x80) != (opcode[1] & 0x80))) {
                state->cc.carry = !state->cc.carry;
            };

            if (opcode[1] == 0) {
                state->cc.carry = 0;
            }
            ans = ans & 0xff;
            state->cc.zero = (ans == 0);
            state->cc.sign = ((ans & 0x80) == 0x80);
            state->cc.parity = parity(ans);
            state->pc += 2;
            cycles = 7;
            break;
        }
        case 0xff:  //RST 7
            state->memory[state->sp - 1] = (byte)((state->pc + 3) >> 8);
            state->memory[state->sp - 2] = (byte)(state->pc + 3);
            state->sp -= 2;
            state->pc = 0x38;
            cycles = 11;
            break;
        default:
            cycles = -1;
            break;
    }
    return cycles;
}