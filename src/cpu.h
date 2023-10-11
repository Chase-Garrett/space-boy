#ifndef CPU_H
#define CPU_H
#include <stdlib.h>

// Condition Codes struct
typedef struct ConditionCodes {
    uint8_t z:1; // Zero
    uint8_t s:1; // Sign
    uint8_t p:1; // Parity
    uint8_t cy:1; // Carry
    uint8_t ac:1; // Auxiliary Carry
    uint8_t pad:3; // Padding
} ConditionCodes;

// State struct
typedef struct State8080 {
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;
    uint16_t sp;
    uint16_t pc;
    uint8_t *memory;
    struct ConditionCodes cc;
    uint8_t int_enable;
} State8080;

// Prototypes
void UnimplementedInstruction(State8080* state);
int Emulate8080Op(State8080* state);

#endif