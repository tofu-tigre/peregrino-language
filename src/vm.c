#include "vm.h"

void init_vm(VM *vm) {

}

void free_vm(VM *vm) {

}

static uint8_t read_byte(uint8_t **ip) {
    uint8_t val = **ip;
    (*ip)++;
    return val;
}

static Value read_constant(VM *vm) {
    uint8_t index = read_byte(&(vm->ip));
    return vm->chunk->constants.values[index];
}

static Value read_constant_long(VM *vm) {
    uint16_t index = (read_byte(&(vm->ip)) << 8) | read_byte(&(vm->ip));
    return vm->chunk->constants.values[index];
}

static InterpretResult run(VM *vm) {
    for(;;) {
        #ifdef DEBUG_TRACE_EXECUTION
        disassemble_instruction(vm->chunk, (int)(vm->ip - vm->chunk->code));
        #endif

        uint8_t instruction;
        switch(instruction = read_byte(&(vm->ip))) {
            case OP_CONSTANT: {
                Value constant = read_constant(vm);
                print_value(constant);
                printf("\n");
                break;
            }
            case OP_CONSTANT_LONG: {
                Value constant = read_constant_long(vm);
                print_value(constant);
                printf("\n");
                break;
            }
            case OP_RETURN: {
                return INTERPRET_OK;
            }
        }
    }
}

InterpretResult interpret(VM *vm, Chunk *chunk) {
    vm->chunk = chunk;
    vm->ip = vm->chunk->code;
    return run(vm);
}