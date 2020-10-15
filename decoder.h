#ifndef DECODER_H
#define DECODER_H

//decode the instruction in buffer at program counter, write to buffer decoded
int decode(unsigned char* buffer, int pc, char* decoded);

#endif