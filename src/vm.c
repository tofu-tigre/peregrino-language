#include "vm.h"
#include "compiler.h"


void init_vm(VM *vm) {
    init_stack(&vm->stack);
    vm->chunk = NULL;
}

void free_vm(VM *vm) {
    free_stack(&vm->stack);
    if(vm->chunk == NULL) return;
    free_chunk(vm->chunk);
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

static Value add(Value a, Value b) {
    return a + b;
}


static Value subtract(Value a, Value b) {
    return a - b;
}


static Value multiply(Value a, Value b) {
    return a * b;
}


static Value divide(Value a, Value b) {
    return a / b;
}


static void binary_op(VM *vm, Value (*op)(Value, Value)) {
    Value b = pop(&vm->stack);
    Value a = pop(&vm->stack);
    push(&vm->stack, op(a, b));
}


static InterpretResult run(VM *vm) {
    for(;;) {
        #ifdef DEBUG_TRACE_EXECUTION
        printf("    ");
        for(Value *slot = vm->stack.data; slot < vm->stack.top; slot++) {
            printf("[ ");
            print_value(*slot);
            printf(" ]");
        }
        printf("\n");
        disassemble_instruction(vm->chunk, (int)(vm->ip - vm->chunk->code));
        #endif

        uint8_t instruction;
        switch(instruction = read_byte(&(vm->ip))) {
            case OP_CONSTANT: {
                Value constant = read_constant(vm);
                push(&vm->stack, constant);
                break;
            }
            case OP_CONSTANT_LONG: {
                Value constant = read_constant_long(vm);
                push(&vm->stack, constant);
                break;
            }
            case OP_NEGATE: {
                push(&vm->stack, -pop(&vm->stack));
                break;
            }
            case OP_ADD: { binary_op(vm, &add); break; }
            case OP_SUBTRACT: { binary_op(vm, &subtract); break; }
            case OP_MULTIPLY: { binary_op(vm, &multiply); break; }
            case OP_DIVIDE: { binary_op(vm, &divide); break; }
            case OP_RETURN: {
                print_value(pop(&vm->stack));
                printf("\n");
                return INTERPRET_OK;
            }
        }
    }
}

InterpretResult interpret(VM *vm, const char *source) {
    compile(vm, source);
    return INTERPRET_OK;
}
