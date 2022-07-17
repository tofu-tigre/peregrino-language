#include <stdarg.h>
#include <stdio.h>
#include "vm.h"
#include "compiler.h"
#include "value.h"

static VM vm;
static double add(double a, double b);
static double subtract(double a, double b);
static double multiply(double a, double b);
static double divide(double a, double b);
static double less(double a, double b);
static double greater(double a, double b);
static uint8_t read_byte();
static Value read_constant();
static Value read_constant_long();
static InterpretResult binary_op(ValueType type, double (*op)(double, double));
static InterpretResult run();
static void runtime_error(const char* format, ...);
static bool is_falsey(Value value);
static bool values_equal(Value a, Value b);

/* Starts up the virtual machine.
 * First it creates a chunk and then writes bytecode
 * to the chunk from the source file using the 
 * scanner and parser. Finally, it passes the bytecode
 * chunk into the virtual machine for interpretation.
 */
InterpretResult interpret(const char* source) {
    Chunk chunk;
    init_chunk(&chunk);
    if(!compile(source, &chunk)) {
        free_chunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }
    
    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpretResult result = run();
    free_chunk(&chunk);
    return result;
}


/* The heart of the virtual machine.
 * Reads the instruction byte code byte-by-byte
 * and evaluates using a stack.
 */
static InterpretResult run() {
    #ifdef DEBUG_TRACE_EXECUTION
        printf("\n===== stack trace =====");
    #endif
    for(;;) {
        #ifdef DEBUG_TRACE_EXECUTION
        printf("    ");
        for(Value *slot = vm.stack.data; slot < vm.stack.top; slot++) {
            printf("[ ");
            print_value(*slot);
            printf(" ]");
        }
        printf("\n");
        disassemble_instruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
        #endif

        uint8_t instruction;
        switch(instruction = read_byte()) {
            case OP_CONSTANT: {
                Value constant = read_constant();
                push(&vm.stack, constant);
                break;
            }
            case OP_CONSTANT_LONG: {
                Value constant = read_constant_long();
                push(&vm.stack, constant);
                break;
            }
            case OP_NEGATE: {
                if(!IS_NUMBER(peek(&vm.stack, 0))) {
                    runtime_error("Operand must be a number.");
                    return INTERPRET_RUNTIME_ERROR;
                }
                push(&vm.stack, NUMBER_VAL(-AS_NUMBER(pop(&vm.stack))));
                break;
            }
            case OP_ADD: { 
                if(binary_op(VAL_NUMBER, &add) == INTERPRET_RUNTIME_ERROR)
                    return INTERPRET_RUNTIME_ERROR;
                break;
            }
            case OP_SUBTRACT: {
                if(binary_op(VAL_NUMBER, &subtract) == INTERPRET_RUNTIME_ERROR)
                    return INTERPRET_RUNTIME_ERROR;
                break;
            }
            case OP_MULTIPLY: {
                if(binary_op(VAL_NUMBER, &multiply) == INTERPRET_RUNTIME_ERROR)
                    return INTERPRET_RUNTIME_ERROR;
                break;
            }
            case OP_DIVIDE: {
                if(binary_op(VAL_NUMBER, &divide) == INTERPRET_RUNTIME_ERROR)
                    return INTERPRET_RUNTIME_ERROR;
                break;
            }
            case OP_NULL: {
                push(&vm.stack, NULL_VAL);
                break;
            }
            case OP_TRUE: {
                push(&vm.stack, BOOL_VAL(true));
                break;
            }
            case OP_FALSE: {
                push(&vm.stack, BOOL_VAL(false));
                break;
            }
            case OP_NOT: {
                push(&vm.stack, BOOL_VAL(is_falsey(pop(&vm.stack))));
                break;
            }
            case OP_EQUAL: {
                Value b = pop(&vm.stack);
                Value a = pop(&vm.stack);
                push(&vm.stack, BOOL_VAL(values_equal(a, b)));
                break;
            }
            case OP_GREATER: {
                if(binary_op(VAL_BOOL, &greater) == INTERPRET_RUNTIME_ERROR)
                    return INTERPRET_RUNTIME_ERROR;
                break;
            }
            case OP_LESS: {
                if(binary_op(VAL_BOOL, &less) == INTERPRET_RUNTIME_ERROR)
                    return INTERPRET_RUNTIME_ERROR;
                break;
            }
            case OP_RETURN: {
                print_value(pop(&vm.stack));
                printf("\n");
                return INTERPRET_OK;
            }
        }
    }
}


static void runtime_error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    size_t instruction = vm.ip - vm.chunk->code - 1;
    int line = get_line(vm.chunk, instruction);
    fprintf(stderr, "[line %d] in script\n", line);
    // free_stack(&vm.stack); Currently freed at end.
}


static bool is_falsey(Value value) {
    return IS_NULL(value) || 
        (IS_BOOL(value) && !AS_BOOL(value)) ||
        (IS_NUMBER(value) && AS_NUMBER(value) == 0);
}

void init_vm() {
    init_stack(&vm.stack);
    vm.chunk = NULL;
}


void free_vm() {
    free_stack(&vm.stack);
    if(vm.chunk == NULL) return;
    free_chunk(vm.chunk);
}


static uint8_t read_byte() {
    return *vm.ip++;
}


static Value read_constant() {
    uint8_t index = read_byte();
    return vm.chunk->constants.values[index];
}


static Value read_constant_long() {
    uint16_t index = (read_byte() << 8) | read_byte();
    return vm.chunk->constants.values[index];
}


static double add(double a, double b) {

    return a + b;
}


static double subtract(double a, double b) {
    return a - b;
}


static double multiply(double a, double b) {
    return a * b;
}


static double divide(double a, double b) {
    return a / b;
}


static double greater(double a, double b) {
    return a > b ? 1. : 0.;
}


static double less(double a, double b) {
    return a < b ? 1. : 0.;
}

static InterpretResult binary_op(ValueType type, double (*op)(double, double)) {
    if(!IS_NUMBER(peek(&vm.stack, 0)) || !IS_NUMBER(peek(&vm.stack, 1))) {
        runtime_error("Operands must be numbers.");
        return INTERPRET_RUNTIME_ERROR;
    }
    double b = AS_NUMBER(pop(&vm.stack));
    double a = AS_NUMBER(pop(&vm.stack));


    switch(type) {
        case VAL_BOOL: {
            bool rv = (op(a, b) == 1 ? true : false);
            push(&vm.stack, BOOL_VAL(rv));
            return INTERPRET_OK;
            break;
        }
        case VAL_NUMBER: {
            push(&vm.stack, NUMBER_VAL(op(a, b)));
            return INTERPRET_OK;
            break;
        }
        default: return INTERPRET_RUNTIME_ERROR;
    }
}


static bool values_equal(Value a, Value b) {
    if(a.type != b.type) return false;
    switch (a.type) {
        case VAL_BOOL: return AS_BOOL(a) == AS_BOOL(b);
        case VAL_NULL: return true;
        case VAL_NUMBER: return AS_NUMBER(a) == AS_NUMBER(b);
        default: return false;
    }
}