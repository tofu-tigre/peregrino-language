#ifndef vm_h
#define vm_h

#include "chunk.h"
#include "common.h"
#include "value.h"
#include "debug.h"
#include "stack.h"



typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} InterpretResult;

typedef struct {
    Chunk *chunk;
    uint8_t *ip;
    Stack stack;
} VM;

void init_vm(VM *vm);
void free_vm(VM *vm);
InterpretResult interpret(VM *vm, const char *source);
#endif