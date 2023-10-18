#include <stdio.h>
#include "cpu.h"

// define unimplemented instruction function
void unimplemented_instruction(State8080 *state) {
  printf("Error: Unimplemented instruction: 0x%02x\n", &state->memory[state->pc]);
  exit(1);
}

// get (M) HL register pair by combining H and L registers
uint8_t get_hl(State8080 *state) {
  return (state->h << 8) | state->l;
}

// set (M) HL register pair by splitting 16-bit value into H and L registers
void set_hl(State8080 *state, uint16_t value) {
  state->h = (value >> 8) & 0xff;
  state->l = value & 0xff;
}

// check parity function
uint8_t parity(uint8_t answer) {
  uint8_t parity = 0;
  for (int i = 0; i < 8; i++) {
    if (answer & (1 << i)) {
      parity ^= 1;
    }
  }

  return parity;
}

// set condition codes function
void set_cc(State8080 *state, uint8_t value) {
  state->cc.z = ((value & 0xff) == 0);
  state->cc.s = ((value & 0x80) != 0);
  state->cc.cy = (value > 0xff);
  state->cc.p = parity(value & 0xff);
}

//********************************************************************************
//                         ARITHMETIC FUNCTIONS                                  *
//********************************************************************************

// define add function
void add(State8080 *state, uint8_t value) {
  uint16_t answer = (uint16_t)state->a + (uint16_t)value;
  set_cc(state, answer);
}

// define adc function
void adc(State8080 *state, uint8_t value) {
  uint16_t answer = (uint16_t)state->a + (uint16_t)value + (uint16_t)state->cc.cy;
  set_cc(state, answer);
}

// define adi function
void adi(State8080 *state) {
  uint8_t value = state->memory[state->pc + 1];
  add(state, value);
}

// define aci function
void aci(State8080 *state) {
  uint8_t value = state->memory[state->pc + 1];
  adc(state, value);
}

// define sub function
void sub(State8080 *state, uint8_t value) {
  uint16_t answer = (uint16_t)state->a - (uint16_t)value;
  set_cc(state, answer);
}

// define sbb function
void sbb(State8080 *state, uint8_t value) {
  uint16_t answer = (uint16_t)state->a - (uint16_t)value - (uint16_t)state->cc.cy;
  set_cc(state, answer);
}

// define sui function
void sui(State8080 *state) {
  uint8_t value = state->memory[state->pc + 1];
  sub(state, value);
}

// define inr function
void inr(State8080 *state, uint8_t *reg) {
  uint16_t answer = (uint16_t)*reg + 1;
  set_cc(state, answer);
  *reg = answer & 0xff;
}

// define dcr function
void dcr(State8080 *state, uint8_t *reg) {
  uint16_t answer = (uint16_t)*reg - 1;
  set_cc(state, answer);
  *reg = answer & 0xff;
}

// define inx function
void inx(State8080 *state, uint8_t *reg1, uint8_t *reg2) {
  uint16_t answer = (uint16_t)*reg1 << 8 | (uint16_t)*reg2;
  answer += 1;
  *reg1 = (answer >> 8) & 0xff;
  *reg2 = answer & 0xff;
}

// define dcx function
void dcx(State8080 *state, uint8_t *reg1, uint8_t *reg2) {
  uint16_t answer = (uint16_t)*reg1 << 8 | (uint16_t)*reg2;
  answer -= 1;
  *reg1 = (answer >> 8) & 0xff;
  *reg2 = answer & 0xff;
}

// define dad function
void dad(State8080 *state, uint8_t *reg1, uint8_t *reg2) {
  uint16_t answer = (uint16_t)*reg1 << 8 | (uint16_t)*reg2;
  answer += get_hl(state);
  state->cc.cy = (answer > 0xffff);
  *reg1 = (answer >> 8) & 0xff;
  *reg2 = answer & 0xff;
}

//********************************************************************************
//                              BRANCH FUNCTIONS                                  *
//********************************************************************************

// define jmp function
void jmp(State8080 *state) {
  uint16_t address = state->memory[state->pc + 1] | (state->memory[state->pc + 2] << 8);
  state->pc = address;
}

// define jnz function
void jnz(State8080 *state) {
  if (!state->cc.z) {
    jmp(state);
  } else {
    state->pc += 2;
  }
}

// define jz function
void jz(State8080 *state) {
  if (state->cc.z) {
    jmp(state);
  } else {
    state->pc += 2;
  }
}

// define jnc function
void jnc(State8080 *state) {
  if (!state->cc.cy) {
    jmp(state);
  } else {
    state->pc += 2;
  }
}

// define jc function
void jc(State8080 *state) {
  if (state->cc.cy) {
    jmp(state);
  } else {
    state->pc += 2;
  }
}

// define jpo function
void jpo(State8080 *state) {
  if (!state->cc.p) {
    jmp(state);
  } else {
    state->pc += 2;
  }
}

// define jpe function
void jpe(State8080 *state) {
  if (state->cc.p) {
    jmp(state);
  } else {
    state->pc += 2;
  }
}

// define jp function
void jp(State8080 *state) {
  if (!state->cc.s) {
    jmp(state);
  } else {
    state->pc += 2;
  }
}

// define jm function
void jm(State8080 *state) {
  if (state->cc.s) {
    jmp(state);
  } else {
    state->pc += 2;
  }
}


//********************************************************************************
//                              EMULATE FUNCTION                                   *
//********************************************************************************
// define emulator function
int emulate8080p(State8080 *state) {
  unsigned char *opcode = &state->memory[state->pc];

  switch (*opcode) {
    // 0x00 - 0x0f
    case 0x00: break;
    case 0x01: unimplemented_instruction(state); break;
    case 0x02: unimplemented_instruction(state); break;
    case 0x03: inx(state, state->b, state->c); break;
    case 0x04: inr(state, state->b); break;
    case 0x05: dcr(state, state->b); break;
    case 0x06: unimplemented_instruction(state); break;
    case 0x07: unimplemented_instruction(state); break;
    case 0x08: break;
    case 0x09: dad(state, state->b, state->c); break;
    case 0x0a: unimplemented_instruction(state); break;
    case 0x0b: dcx(state, state->b, state->c); break;
    case 0x0c: inr(state, state->c); break;
    case 0x0d: dcr(state, state->c); break;
    case 0x0e: unimplemented_instruction(state); break;
    case 0x0f: unimplemented_instruction(state); break;

    // 0x10 - 0x1f
    case 0x10: break;
    case 0x11: unimplemented_instruction(state); break;
    case 0x12: unimplemented_instruction(state); break;
    case 0x13: inx(state, state->d, state->e); break;
    case 0x14: inr(state, state->d); break;
    case 0x15: dcr(state, state->d); break;
    case 0x16: unimplemented_instruction(state); break;
    case 0x17: unimplemented_instruction(state); break;
    case 0x18: break;
    case 0x19: dad(state, state->d, state->e); break;
    case 0x1a: unimplemented_instruction(state); break;
    case 0x1b: dcx(state, state->d, state->e); break;
    case 0x1c: inr(state, state->e); break;
    case 0x1d: dcr(state, state->e); break;
    case 0x1e: unimplemented_instruction(state); break;
    case 0x1f: unimplemented_instruction(state); break;

    // 0x20 - 0x2f
    case 0x20: break;
    case 0x21: unimplemented_instruction(state); break;
    case 0x22: unimplemented_instruction(state); break;
    case 0x23: inx(state, state->h, state->l); break;
    case 0x24: inr(state, state->h); break;
    case 0x25: dcr(state, state->h); break;
    case 0x26: unimplemented_instruction(state); break;
    case 0x27: unimplemented_instruction(state); break;
    case 0x28: break;
    case 0x29: dad(state, state->h, state->l); break;
    case 0x2a: unimplemented_instruction(state); break;
    case 0x2b: dcx(state, state->h, state->l); break;
    case 0x2c: inr(state, state->l); break;
    case 0x2d: dcr(state, state->l); break;
    case 0x2e: unimplemented_instruction(state); break;
    case 0x2f: unimplemented_instruction(state); break;

    // 0x30 - 0x3f
    case 0x30: break;
    case 0x31: unimplemented_instruction(state); break;
    case 0x32: unimplemented_instruction(state); break;
    case 0x33: state->sp++; break;
    case 0x34: inr(state, get_hl(state)); break;
    case 0x35: dcr(state, get_hl(state)); break;
    case 0x36: unimplemented_instruction(state); break;
    case 0x37: unimplemented_instruction(state); break;
    case 0x38: break;
    case 0x39: dad(state, get_hl(state), state->sp); break;
    case 0x3a: unimplemented_instruction(state); break;
    case 0x3b: state->sp--; break;
    case 0x3c: inr(state, state->a); break;
    case 0x3d: dcr(state, state->a); break;
    case 0x3e: unimplemented_instruction(state); break;
    case 0x3f: unimplemented_instruction(state); break;

    // 0x40 - 0x4f
    case 0x40: unimplemented_instruction(state); break;
    case 0x41: unimplemented_instruction(state); break;
    case 0x42: unimplemented_instruction(state); break;
    case 0x43: unimplemented_instruction(state); break;
    case 0x44: unimplemented_instruction(state); break;
    case 0x45: unimplemented_instruction(state); break;
    case 0x46: unimplemented_instruction(state); break;
    case 0x47: unimplemented_instruction(state); break;
    case 0x48: unimplemented_instruction(state); break;
    case 0x49: unimplemented_instruction(state); break;
    case 0x4a: unimplemented_instruction(state); break;
    case 0x4b: unimplemented_instruction(state); break;
    case 0x4c: unimplemented_instruction(state); break;
    case 0x4d: unimplemented_instruction(state); break;
    case 0x4e: unimplemented_instruction(state); break;
    case 0x4f: unimplemented_instruction(state); break;

    // 0x50 - 0x5f
    case 0x50: unimplemented_instruction(state); break;
    case 0x51: unimplemented_instruction(state); break;
    case 0x52: unimplemented_instruction(state); break;
    case 0x53: unimplemented_instruction(state); break;
    case 0x54: unimplemented_instruction(state); break;
    case 0x55: unimplemented_instruction(state); break;
    case 0x56: unimplemented_instruction(state); break;
    case 0x57: unimplemented_instruction(state); break;
    case 0x58: unimplemented_instruction(state); break;
    case 0x59: unimplemented_instruction(state); break;
    case 0x5a: unimplemented_instruction(state); break;
    case 0x5b: unimplemented_instruction(state); break;
    case 0x5c: unimplemented_instruction(state); break;
    case 0x5d: unimplemented_instruction(state); break;
    case 0x5e: unimplemented_instruction(state); break;
    case 0x5f: unimplemented_instruction(state); break;

    // 0x60 - 0x6f
    case 0x60: unimplemented_instruction(state); break;
    case 0x61: unimplemented_instruction(state); break;
    case 0x62: unimplemented_instruction(state); break;
    case 0x63: unimplemented_instruction(state); break;
    case 0x64: unimplemented_instruction(state); break;
    case 0x65: unimplemented_instruction(state); break;
    case 0x66: unimplemented_instruction(state); break;
    case 0x67: unimplemented_instruction(state); break;
    case 0x68: unimplemented_instruction(state); break;
    case 0x69: unimplemented_instruction(state); break;
    case 0x6a: unimplemented_instruction(state); break;
    case 0x6b: unimplemented_instruction(state); break;
    case 0x6c: unimplemented_instruction(state); break;
    case 0x6d: unimplemented_instruction(state); break;
    case 0x6e: unimplemented_instruction(state); break;
    case 0x6f: unimplemented_instruction(state); break;

    // 0x70 - 0x7f
    case 0x70: unimplemented_instruction(state); break;
    case 0x71: unimplemented_instruction(state); break;
    case 0x72: unimplemented_instruction(state); break;
    case 0x73: unimplemented_instruction(state); break;
    case 0x74: unimplemented_instruction(state); break;
    case 0x75: unimplemented_instruction(state); break;
    case 0x76: unimplemented_instruction(state); break;
    case 0x77: unimplemented_instruction(state); break;
    case 0x78: unimplemented_instruction(state); break;
    case 0x79: unimplemented_instruction(state); break;
    case 0x7a: unimplemented_instruction(state); break;
    case 0x7b: unimplemented_instruction(state); break;
    case 0x7c: unimplemented_instruction(state); break;
    case 0x7d: unimplemented_instruction(state); break;
    case 0x7e: unimplemented_instruction(state); break;
    case 0x7f: unimplemented_instruction(state); break;

    // 0x80 - 0x8f
    case 0x80: add(state, state->b); break;
    case 0x81: add(state, state->c); break;
    case 0x82: add(state, state->d); break;
    case 0x83: add(state, state->e); break;
    case 0x84: add(state, state->h); break;
    case 0x85: add(state, state->l); break;
    case 0x86: add(state, get_hl(state)); break;
    case 0x87: add(state, state->a); break;
    case 0x88: adc(state, state->b); break;
    case 0x89: adc(state, state->c); break;
    case 0x8a: adc(state, state->d); break;
    case 0x8b: adc(state, state->e); break;
    case 0x8c: adc(state, state->h); break;
    case 0x8d: adc(state, state->l); break;
    case 0x8e: adc(state, get_hl(state)); break;
    case 0x8f: adc(state, state->a); break;

    // 0x90 - 0x9f
    case 0x90: sub(state, state->b); break;
    case 0x91: sub(state, state->c); break;
    case 0x92: sub(state, state->d); break;
    case 0x93: sub(state, state->e); break;
    case 0x94: sub(state, state->h); break;
    case 0x95: sub(state, state->l); break;
    case 0x96: sub(state, get_hl(state)); break;
    case 0x97: sub(state, state->a); break;
    case 0x98: sbb(state, state->b); break;
    case 0x99: sbb(state, state->c); break;
    case 0x9a: sbb(state, state->d); break;
    case 0x9b: sbb(state, state->e); break;
    case 0x9c: sbb(state, state->h); break;
    case 0x9d: sbb(state, state->l); break;
    case 0x9e: sbb(state, get_hl(state)); break;
    case 0x9f: sbb(state, state->a); break;

    // 0xa0 - 0xaf
    case 0xa0: unimplemented_instruction(state); break;
    case 0xa1: unimplemented_instruction(state); break;
    case 0xa2: unimplemented_instruction(state); break;
    case 0xa3: unimplemented_instruction(state); break;
    case 0xa4: unimplemented_instruction(state); break;
    case 0xa5: unimplemented_instruction(state); break;
    case 0xa6: unimplemented_instruction(state); break;
    case 0xa7: unimplemented_instruction(state); break;
    case 0xa8: unimplemented_instruction(state); break;
    case 0xa9: unimplemented_instruction(state); break;
    case 0xaa: unimplemented_instruction(state); break;
    case 0xab: unimplemented_instruction(state); break;
    case 0xac: unimplemented_instruction(state); break;
    case 0xad: unimplemented_instruction(state); break;
    case 0xae: unimplemented_instruction(state); break;
    case 0xaf: unimplemented_instruction(state); break;

    // 0xb0 - 0xbf
    case 0xb0: unimplemented_instruction(state); break;
    case 0xb1: unimplemented_instruction(state); break;
    case 0xb2: unimplemented_instruction(state); break;
    case 0xb3: unimplemented_instruction(state); break;
    case 0xb4: unimplemented_instruction(state); break;
    case 0xb5: unimplemented_instruction(state); break;
    case 0xb6: unimplemented_instruction(state); break;
    case 0xb7: unimplemented_instruction(state); break;
    case 0xb8: unimplemented_instruction(state); break;
    case 0xb9: unimplemented_instruction(state); break;
    case 0xba: unimplemented_instruction(state); break;
    case 0xbb: unimplemented_instruction(state); break;
    case 0xbc: unimplemented_instruction(state); break;
    case 0xbd: unimplemented_instruction(state); break;
    case 0xbe: unimplemented_instruction(state); break;
    case 0xbf: unimplemented_instruction(state); break;

    // 0xc0 - 0xcf
    case 0xc0: unimplemented_instruction(state); break;
    case 0xc1: unimplemented_instruction(state); break;
    case 0xc2: jnz(state); break;
    case 0xc3: jmp(state); break;
    case 0xc4: unimplemented_instruction(state); break;
    case 0xc5: unimplemented_instruction(state); break;
    case 0xc6: adi(state); break;
    case 0xc7: unimplemented_instruction(state); break;
    case 0xc8: unimplemented_instruction(state); break;
    case 0xc9: unimplemented_instruction(state); break;
    case 0xca: jz(state); break;
    case 0xcb: jmp(state); break;
    case 0xcc: unimplemented_instruction(state); break;
    case 0xcd: unimplemented_instruction(state); break;
    case 0xce: aci(state); break;
    case 0xcf: unimplemented_instruction(state); break;

    // 0xd0 - 0xdf
    case 0xd0: unimplemented_instruction(state); break;
    case 0xd1: unimplemented_instruction(state); break;
    case 0xd2: (state); break;
    case 0xd3: unimplemented_instruction(state); break;
    case 0xd4: unimplemented_instruction(state); break;
    case 0xd5: unimplemented_instruction(state); break;
    case 0xd6: sui(state); break;
    case 0xd7: unimplemented_instruction(state); break;
    case 0xd8: unimplemented_instruction(state); break;
    case 0xd9: unimplemented_instruction(state); break;
    case 0xda: jc(state); break;
    case 0xdb: unimplemented_instruction(state); break;
    case 0xdc: unimplemented_instruction(state); break;
    case 0xdd: unimplemented_instruction(state); break;
    case 0xde: unimplemented_instruction(state); break;
    case 0xdf: unimplemented_instruction(state); break;

    // 0xe0 - 0xef
    case 0xe0: unimplemented_instruction(state); break;
    case 0xe1: unimplemented_instruction(state); break;
    case 0xe2: jpo(state); break;
    case 0xe3: unimplemented_instruction(state); break;
    case 0xe4: unimplemented_instruction(state); break;
    case 0xe5: unimplemented_instruction(state); break;
    case 0xe6: unimplemented_instruction(state); break;
    case 0xe7: unimplemented_instruction(state); break;
    case 0xe8: unimplemented_instruction(state); break;
    case 0xe9: unimplemented_instruction(state); break;
    case 0xea: jpe(state); break;
    case 0xeb: unimplemented_instruction(state); break;
    case 0xec: unimplemented_instruction(state); break;
    case 0xed: unimplemented_instruction(state); break;
    case 0xee: unimplemented_instruction(state); break;
    case 0xef: unimplemented_instruction(state); break;

    // 0xf0- 0xff
    case 0xf0: unimplemented_instruction(state); break;
    case 0xf1: unimplemented_instruction(state); break;
    case 0xf2: jp(state); break;
    case 0xf3: unimplemented_instruction(state); break;
    case 0xf4: unimplemented_instruction(state); break;
    case 0xf5: unimplemented_instruction(state); break;
    case 0xf6: unimplemented_instruction(state); break;
    case 0xf7: unimplemented_instruction(state); break;
    case 0xf8: unimplemented_instruction(state); break;
    case 0xf9: unimplemented_instruction(state); break;
    case 0xfa: jm(state); break;
    case 0xfb: unimplemented_instruction(state); break;
    case 0xfc: unimplemented_instruction(state); break;
    case 0xfd: unimplemented_instruction(state); break;
    case 0xfe: unimplemented_instruction(state); break;
    case 0xff: unimplemented_instruction(state); break;
  }
}
