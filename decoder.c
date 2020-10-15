#include "decoder.h"

#include <stdio.h>

int decode(unsigned char* buffer, int pc, char* decoded) {
    unsigned char* opcode = &buffer[pc];
    int oplen = 1;
    switch (*opcode) {
        case 0x00:
            sprintf(decoded, "NOP");
            break;
        case 0x01:
            oplen = 3;
            sprintf(decoded, "LXI  B, #0x%02x%02x", *(opcode + 2), *(opcode + 1));
            break;
        case 0x02:
            sprintf(decoded, "STAX B");
            break;
        case 0x03:
            sprintf(decoded, "INX  B");
            break;
        case 0x04:
            sprintf(decoded, "INR  B");
            break;
        case 0x05:
            sprintf(decoded, "DCR  B");
            break;
        case 0x06:
            oplen = 2;
            sprintf(decoded, "MVI  B, #0x%02x", *(opcode + 1));
            break;
        case 0x07:
            sprintf(decoded, "RLC");
            break;
        case 0x08:
            sprintf(decoded, "NOP");
            break;
        case 0x09:
            sprintf(decoded, "DAD  B");
            break;
        case 0x0a:
            sprintf(decoded, "LDAX B");
            break;
        case 0x0b:
            sprintf(decoded, "DCX  B");
            break;
        case 0x0c:
            sprintf(decoded, "INR  C");
            break;
        case 0x0d:
            sprintf(decoded, "DCR  C");
            break;
        case 0x0e:
            oplen = 2;
            sprintf(decoded, "MVI  C, #0x%02x", *(opcode + 1));
            break;
        case 0x0f:
            sprintf(decoded, "RRC");
            break;
        case 0x10:
            sprintf(decoded, "NOP");
            break;
        case 0x11:
            oplen = 3;
            sprintf(decoded, "LXI  D, #0x%02x%02x", *(opcode + 2), *(opcode + 1));
            break;
        case 0x12:
            sprintf(decoded, "STAX D");
            break;
        case 0x13:
            sprintf(decoded, "INX  D");
            break;
        case 0x14:
            sprintf(decoded, "INR  D");
            break;
        case 0x15:
            sprintf(decoded, "DCR  D");
            break;
        case 0x16:
            oplen = 2;
            sprintf(decoded, "MVI  D, #0x%02x", *(opcode + 1));
            break;
        case 0x17:
            sprintf(decoded, "RAL");
            break;
        case 0x18:
            sprintf(decoded, "NOP");
            break;
        case 0x19:
            sprintf(decoded, "DAD  D");
            break;
        case 0x1a:
            sprintf(decoded, "LDAX D");
            break;
        case 0x1b:
            sprintf(decoded, "DCX  D");
            break;
        case 0x1c:
            sprintf(decoded, "INR  E");
            break;
        case 0x1d:
            sprintf(decoded, "DCR  E");
            break;
        case 0x1e:
            oplen = 2;
            sprintf(decoded, "MVI  E, #0x%02x", *(opcode + 1));
            break;
        case 0x1f:
            sprintf(decoded, "RAR");
            break;
        case 0x20:
            sprintf(decoded, "NOP");
            break;
        case 0x21:
            oplen = 3;
            sprintf(decoded, "LXI  H, #0x%02x%02x", *(opcode + 2), *(opcode + 1));
            break;
        case 0x22:
            oplen = 3;
            sprintf(decoded, "SHLD $%02x%02x", *(opcode + 2), *(opcode + 1));
            break;
        case 0x23:
            sprintf(decoded, "INX  H");
            break;
        case 0x24:
            sprintf(decoded, "INR  H");
            break;
        case 0x25:
            sprintf(decoded, "DCR  H");
            break;
        case 0x26:
            oplen = 2;
            sprintf(decoded, "MVI  H, #0x%02x", *(opcode + 1));
            break;
        case 0x27:
            sprintf(decoded, "DAA");
            break;
        case 0x28:
            sprintf(decoded, "NOP");
            break;
        case 0x29:
            sprintf(decoded, "DAD  H");
            break;
        case 0x2a:
            oplen = 3;
            sprintf(decoded, "LHLD $%02x%02x", *(opcode + 2), *(opcode + 1));
            break;
        case 0x2b:
            sprintf(decoded, "DCX  H");
            break;
        case 0x2c:
            sprintf(decoded, "INR  L");
            break;
        case 0x2d:
            sprintf(decoded, "DCR  L");
            break;
        case 0x2e:
            oplen = 2;
            sprintf(decoded, "MVI  L, #0x%02x", *(opcode + 1));
            break;
        case 0x2f:
            sprintf(decoded, "CMA");
            break;
        case 0x30:
            sprintf(decoded, "NOP");
            break;
        case 0x31:
            oplen = 3;
            sprintf(decoded, "LXI  SP, #0x%02x%02x", *(opcode + 2), *(opcode + 1));
            break;
        case 0x32:
            oplen = 3;
            sprintf(decoded, "STA  $%02x%02x", *(opcode + 2), *(opcode + 1));
            break;
        case 0x33:
            sprintf(decoded, "INX  SP");
            break;
        case 0x34:
            sprintf(decoded, "INR  M");
            break;
        case 0x35:
            sprintf(decoded, "DCR  M");
            break;
        case 0x36:
            oplen = 2;
            sprintf(decoded, "MVI  M, #0x%02x", *(opcode + 1));
            break;
        case 0x37:
            sprintf(decoded, "STC");
            break;
        case 0x38:
            sprintf(decoded, "NOP");
            break;
        case 0x39:
            sprintf(decoded, "DAD  SP");
            break;
        case 0x3a:
            oplen = 3;
            sprintf(decoded, "LDA  $%02x%02x", *(opcode + 2), *(opcode + 1));
            break;
        case 0x3b:
            sprintf(decoded, "DCX  SP");
            break;
        case 0x3c:
            sprintf(decoded, "INR  A");
            break;
        case 0x3d:
            sprintf(decoded, "DCR  A");
            break;
        case 0x3e:
            oplen = 2;
            sprintf(decoded, "MVI  A, #0x%02x", *(opcode + 1));
            break;
        case 0x3f:
            sprintf(decoded, "CMC");
            break;
        case 0x40:
            sprintf(decoded, "MOV  B, B");
            break;
        case 0x41:
            sprintf(decoded, "MOV  B, C");
            break;
        case 0x42:
            sprintf(decoded, "MOV  B, D");
            break;
        case 0x43:
            sprintf(decoded, "MOV  B, E");
            break;
        case 0x44:
            sprintf(decoded, "MOV  B, H");
            break;
        case 0x45:
            sprintf(decoded, "MOV  B, L");
            break;
        case 0x46:
            sprintf(decoded, "MOV  B, M");
            break;
        case 0x47:
            sprintf(decoded, "MOV  B, A");
            break;
        case 0x48:
            sprintf(decoded, "MOV  C, B");
            break;
        case 0x49:
            sprintf(decoded, "MOV  C, C");
            break;
        case 0x4a:
            sprintf(decoded, "MOV  C, D");
            break;
        case 0x4b:
            sprintf(decoded, "MOV  C, E");
            break;
        case 0x4c:
            sprintf(decoded, "MOV  C, H");
            break;
        case 0x4d:
            sprintf(decoded, "MOV  C, L");
            break;
        case 0x4e:
            sprintf(decoded, "MOV  C, M");
            break;
        case 0x4f:
            sprintf(decoded, "MOV  C, A");
            break;
        case 0x50:
            sprintf(decoded, "MOV  D, B");
            break;
        case 0x51:
            sprintf(decoded, "MOV  D, C");
            break;
        case 0x52:
            sprintf(decoded, "MOV  D, D");
            break;
        case 0x53:
            sprintf(decoded, "MOV  D, E");
            break;
        case 0x54:
            sprintf(decoded, "MOV  D, H");
            break;
        case 0x55:
            sprintf(decoded, "MOV  D, L");
            break;
        case 0x56:
            sprintf(decoded, "MOV  D, M");
            break;
        case 0x57:
            sprintf(decoded, "MOV  D, A");
            break;
        case 0x58:
            sprintf(decoded, "MOV  E, B");
            break;
        case 0x59:
            sprintf(decoded, "MOV  E, C");
            break;
        case 0x5a:
            sprintf(decoded, "MOV  E, D");
            break;
        case 0x5b:
            sprintf(decoded, "MOV  E, E");
            break;
        case 0x5c:
            sprintf(decoded, "MOV  E, H");
            break;
        case 0x5d:
            sprintf(decoded, "MOV  E, L");
            break;
        case 0x5e:
            sprintf(decoded, "MOV  E, M");
            break;
        case 0x5f:
            sprintf(decoded, "MOV  E, A");
            break;
        case 0x60:
            sprintf(decoded, "MOV  H, B");
            break;
        case 0x61:
            sprintf(decoded, "MOV  H, C");
            break;
        case 0x62:
            sprintf(decoded, "MOV  H, D");
            break;
        case 0x63:
            sprintf(decoded, "MOV  H, E");
            break;
        case 0x64:
            sprintf(decoded, "MOV  H, H");
            break;
        case 0x65:
            sprintf(decoded, "MOV  H, L");
            break;
        case 0x66:
            sprintf(decoded, "MOV  H, M");
            break;
        case 0x67:
            sprintf(decoded, "MOV  H, A");
            break;
        case 0x68:
            sprintf(decoded, "MOV  L, B");
            break;
        case 0x69:
            sprintf(decoded, "MOV  L, C");
            break;
        case 0x6a:
            sprintf(decoded, "MOV  L, D");
            break;
        case 0x6b:
            sprintf(decoded, "MOV  L, E");
            break;
        case 0x6c:
            sprintf(decoded, "MOV  L, H");
            break;
        case 0x6d:
            sprintf(decoded, "MOV  L, L");
            break;
        case 0x6e:
            sprintf(decoded, "MOV  L, M");
            break;
        case 0x6f:
            sprintf(decoded, "MOV  L, A");
            break;
        case 0x70:
            sprintf(decoded, "MOV  M, B");
            break;
        case 0x71:
            sprintf(decoded, "MOV  M, C");
            break;
        case 0x72:
            sprintf(decoded, "MOV  M, D");
            break;
        case 0x73:
            sprintf(decoded, "MOV  M, E");
            break;
        case 0x74:
            sprintf(decoded, "MOV  M, H");
            break;
        case 0x75:
            sprintf(decoded, "MOV  M, L");
            break;
        case 0x76:
            sprintf(decoded, "HLT");
            break;
        case 0x77:
            sprintf(decoded, "MOV  M, A");
            break;
        case 0x78:
            sprintf(decoded, "MOV  A, B");
            break;
        case 0x79:
            sprintf(decoded, "MOV  A, C");
            break;
        case 0x7a:
            sprintf(decoded, "MOV  A, D");
            break;
        case 0x7b:
            sprintf(decoded, "MOV  A, E");
            break;
        case 0x7c:
            sprintf(decoded, "MOV  A, H");
            break;
        case 0x7d:
            sprintf(decoded, "MOV  A, L");
            break;
        case 0x7e:
            sprintf(decoded, "MOV  A, M");
            break;
        case 0x7f:
            sprintf(decoded, "MOV  A, A");
            break;
        case 0x80:
            sprintf(decoded, "ADD  B");
            break;
        case 0x81:
            sprintf(decoded, "ADD  C");
            break;
        case 0x82:
            sprintf(decoded, "ADD  D");
            break;
        case 0x83:
            sprintf(decoded, "ADD  E");
            break;
        case 0x84:
            sprintf(decoded, "ADD  H");
            break;
        case 0x85:
            sprintf(decoded, "ADD  L");
            break;
        case 0x86:
            sprintf(decoded, "ADD  M");
            break;
        case 0x87:
            sprintf(decoded, "ADD  A");
            break;
        case 0x88:
            sprintf(decoded, "ADC  B");
            break;
        case 0x89:
            sprintf(decoded, "ADC  C");
            break;
        case 0x8a:
            sprintf(decoded, "ADC  D");
            break;
        case 0x8b:
            sprintf(decoded, "ADC  E");
            break;
        case 0x8c:
            sprintf(decoded, "ADC  H");
            break;
        case 0x8d:
            sprintf(decoded, "ADC  L");
            break;
        case 0x8e:
            sprintf(decoded, "ADC  M");
            break;
        case 0x8f:
            sprintf(decoded, "ADC  A");
            break;
        case 0x90:
            sprintf(decoded, "SUB  B");
            break;
        case 0x91:
            sprintf(decoded, "SUB  C");
            break;
        case 0x92:
            sprintf(decoded, "SUB  D");
            break;
        case 0x93:
            sprintf(decoded, "SUB  E");
            break;
        case 0x94:
            sprintf(decoded, "SUB  H");
            break;
        case 0x95:
            sprintf(decoded, "SUB  L");
            break;
        case 0x96:
            sprintf(decoded, "SUB  M");
            break;
        case 0x97:
            sprintf(decoded, "SUB  A");
            break;
        case 0x98:
            sprintf(decoded, "SBB  B");
            break;
        case 0x99:
            sprintf(decoded, "SBB  C");
            break;
        case 0x9a:
            sprintf(decoded, "SBB  D");
            break;
        case 0x9b:
            sprintf(decoded, "SBB  E");
            break;
        case 0x9c:
            sprintf(decoded, "SBB  H");
            break;
        case 0x9d:
            sprintf(decoded, "SBB  L");
            break;
        case 0x9e:
            sprintf(decoded, "SBB  M");
            break;
        case 0x9f:
            sprintf(decoded, "SBB  A");
            break;
        case 0xa0:
            sprintf(decoded, "ANA  B");
            break;
        case 0xa1:
            sprintf(decoded, "ANA  C");
            break;
        case 0xa2:
            sprintf(decoded, "ANA  D");
            break;
        case 0xa3:
            sprintf(decoded, "ANA  E");
            break;
        case 0xa4:
            sprintf(decoded, "ANA  H");
            break;
        case 0xa5:
            sprintf(decoded, "ANA  L");
            break;
        case 0xa6:
            sprintf(decoded, "ANA  M");
            break;
        case 0xa7:
            sprintf(decoded, "ANA  A");
            break;
        case 0xa8:
            sprintf(decoded, "XRA  B");
            break;
        case 0xa9:
            sprintf(decoded, "XRA  C");
            break;
        case 0xaa:
            sprintf(decoded, "XRA  D");
            break;
        case 0xab:
            sprintf(decoded, "XRA  E");
            break;
        case 0xac:
            sprintf(decoded, "XRA  H");
            break;
        case 0xad:
            sprintf(decoded, "XRA  L");
            break;
        case 0xae:
            sprintf(decoded, "XRA  M");
            break;
        case 0xaf:
            sprintf(decoded, "XRA  A");
            break;
        case 0xb0:
            sprintf(decoded, "ORA  B");
            break;
        case 0xb1:
            sprintf(decoded, "ORA  C");
            break;
        case 0xb2:
            sprintf(decoded, "ORA  D");
            break;
        case 0xb3:
            sprintf(decoded, "ORA  E");
            break;
        case 0xb4:
            sprintf(decoded, "ORA  H");
            break;
        case 0xb5:
            sprintf(decoded, "ORA  L");
            break;
        case 0xb6:
            sprintf(decoded, "ORA  M");
            break;
        case 0xb7:
            sprintf(decoded, "ORA  A");
            break;
        case 0xb8:
            sprintf(decoded, "CMP  B");
            break;
        case 0xb9:
            sprintf(decoded, "CMP  C");
            break;
        case 0xba:
            sprintf(decoded, "CMP  D");
            break;
        case 0xbb:
            sprintf(decoded, "CMP  E");
            break;
        case 0xbc:
            sprintf(decoded, "CMP  H");
            break;
        case 0xbd:
            sprintf(decoded, "CMP  L");
            break;
        case 0xbe:
            sprintf(decoded, "CMP  M");
            break;
        case 0xbf:
            sprintf(decoded, "CMP  A");
            break;
        case 0xc0:
            sprintf(decoded, "RNZ");
            break;
        case 0xc1:
            sprintf(decoded, "POP  B");
            break;
        case 0xc2:
            oplen = 3;
            sprintf(decoded, "JNZ  $%02x%02x", *(opcode + 2), *(opcode + 1));
            break;
        case 0xc3:
            oplen = 3;
            sprintf(decoded, "JMP  $%02x%02x", *(opcode + 2), *(opcode + 1));
            break;
        case 0xc4:
            oplen = 3;
            sprintf(decoded, "CNZ  $%02x%02x", *(opcode + 2), *(opcode + 1));
            break;
        case 0xc5:
            sprintf(decoded, "PUSH B");
            break;
        case 0xc6:
            oplen = 2;
            sprintf(decoded, "ADI  #0x%02x", *(opcode + 1));
            break;
        case 0xc7:
            sprintf(decoded, "RST  0");
            break;
        case 0xc8:
            sprintf(decoded, "RZ");
            break;
        case 0xc9:
            sprintf(decoded, "RET");
            break;
        case 0xca:
            oplen = 3;
            sprintf(decoded, "JZ   $%02x%02x", *(opcode + 2), *(opcode + 1));
            break;
        case 0xcb:
            sprintf(decoded, "NOP");
            break;
        case 0xcc:
            oplen = 3;
            sprintf(decoded, "CZ   $%02x%02x", *(opcode + 2), *(opcode + 1));
            break;
        case 0xcd:
            oplen = 3;
            sprintf(decoded, "CALL $%02x%02x", *(opcode + 2), *(opcode + 1));
            break;
        case 0xce:
            oplen = 2;
            sprintf(decoded, "ACI  #0x%02x", *(opcode + 1));
            break;
        case 0xcf:
            sprintf(decoded, "RST  1");
            break;
        case 0xd0:
            sprintf(decoded, "RNC");
            break;
        case 0xd1:
            sprintf(decoded, "POP  D");
            break;
        case 0xd2:
            oplen = 3;
            sprintf(decoded, "JNC  $%02x%02x", *(opcode + 2), *(opcode + 1));
            break;
        case 0xd3:
            oplen = 2;
            sprintf(decoded, "OUT  $%02x", *(opcode + 1));
            break;
        case 0xd4:
            oplen = 3;
            sprintf(decoded, "CNC  $%02x%02x", *(opcode + 2), *(opcode + 1));
            break;
        case 0xd5:
            sprintf(decoded, "PUSH D");
            break;
        case 0xd6:
            oplen = 2;
            sprintf(decoded, "SUI  #0x%02x", *(opcode + 1));
            break;
        case 0xd7:
            sprintf(decoded, "RST  2");
            break;
        case 0xd8:
            sprintf(decoded, "RC");
            break;
        case 0xd9:
            sprintf(decoded, "NOP");
            break;
        case 0xda:
            oplen = 3;
            sprintf(decoded, "JC   $%02x%02x", *(opcode + 2), *(opcode + 1));
            break;
        case 0xdb:
            oplen = 2;
            sprintf(decoded, "IN   #0x%02x", *(opcode + 1));
            break;
        case 0xdc:
            oplen = 3;
            sprintf(decoded, "CC   $%02x%02x", *(opcode + 2), *(opcode + 1));
            break;
        case 0xdd:
            sprintf(decoded, "NOP");
            break;
        case 0xde:
            oplen = 2;
            sprintf(decoded, "SBI  #0x%02x", *(opcode + 1));
            break;
        case 0xdf:
            sprintf(decoded, "RST  3");
            break;
        case 0xe0:
            sprintf(decoded, "RPO");
            break;
        case 0xe1:
            sprintf(decoded, "POP  H");
            break;
        case 0xe2:
            oplen = 3;
            sprintf(decoded, "JPO  $%02x%02x", *(opcode + 2), *(opcode + 1));
            break;
        case 0xe3:
            sprintf(decoded, "XTHL");
            break;
        case 0xe4:
            oplen = 3;
            sprintf(decoded, "CPO  $%02x%02x", *(opcode + 2), *(opcode + 1));
            break;
        case 0xe5:
            sprintf(decoded, "PUSH H");
            break;
        case 0xe6:
            oplen = 2;
            sprintf(decoded, "ANI  #0x%02x", *(opcode + 1));
            break;
        case 0xe7:
            sprintf(decoded, "RST  4");
            break;
        case 0xe8:
            sprintf(decoded, "RPE");
            break;
        case 0xe9:
            sprintf(decoded, "PCHL");
            break;
        case 0xea:
            oplen = 3;
            sprintf(decoded, "JPE  $%02x%02x", *(opcode + 2), *(opcode + 1));
            break;
        case 0xeb:
            sprintf(decoded, "XCHG");
            break;
        case 0xec:
            oplen = 3;
            sprintf(decoded, "CPE  $%02x%02x", *(opcode + 2), *(opcode + 1));
            break;
        case 0xed:
            sprintf(decoded, "NOP");
            break;
        case 0xee:
            oplen = 2;
            sprintf(decoded, "XRI  #0x%02x", *(opcode + 1));
            break;
        case 0xef:
            sprintf(decoded, "RST  5");
            break;
        case 0xf0:
            sprintf(decoded, "RP");
            break;
        case 0xf1:
            sprintf(decoded, "POP  PSW");
            break;
        case 0xf2:
            oplen = 3;
            sprintf(decoded, "JP  $%02x%02x", *(opcode + 2), *(opcode + 1));
            break;
        case 0xf3:
            sprintf(decoded, "DI");
            break;
        case 0xf4:
            oplen = 3;
            sprintf(decoded, "CP  $%02x%02x", *(opcode + 2), *(opcode + 1));
            break;
        case 0xf5:
            sprintf(decoded, "PUSH  PSW");
            break;
        case 0xf6:
            oplen = 2;
            sprintf(decoded, "ORI  #0x%02x", *(opcode + 1));
            break;
        case 0xf7:
            sprintf(decoded, "RST  6");
            break;
        case 0xf8:
            sprintf(decoded, "RM");
            break;
        case 0xf9:
            sprintf(decoded, "SPHL");
            break;
        case 0xfa:
            oplen = 3;
            sprintf(decoded, "JM   $%02x%02x", *(opcode + 2), *(opcode + 1));
            break;
        case 0xfb:
            sprintf(decoded, "EI");
            break;
        case 0xfc:
            oplen = 3;
            sprintf(decoded, "CM  $%02x%02x", *(opcode + 2), *(opcode + 1));
            break;
        case 0xfd:
            sprintf(decoded, "NOP");
            break;
        case 0xfe:
            oplen = 2;
            sprintf(decoded, "CPI  #0x%02x", *(opcode + 1));
            break;
        case 0xff:
            sprintf(decoded, "RST  7");
            break;
        default:
            sprintf(decoded, "INSTRUCTION NOT IMPLEMENTED");
            break;
    }
    return oplen;
}
