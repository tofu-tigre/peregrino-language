#include "vm.h"
#include "compiler.h"

static VM vm;
static Value add(Value a, Value b);
static Value subtract(Value a, Value b);
static Value multiply(Value a, Value b);
static Value divide(Value a, Value b);
static uint8_t read_byte();
static Value read_constant();
static Value read_constant_long();
static void binary_op(Value (*op)(Value, Value));
static InterpretResult run();

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
                push(&vm.stack, -pop(&vm.stack));
                break;
            }
            case OP_ADD: { binary_op(&add); break; }
            case OP_SUBTRACT: { binary_op(&subtract); break; }
            case OP_MULTIPLY: { binary_op(&multiply); break; }
            case OP_DIVIDE: { binary_op(&divide); break; }
            case OP_RETURN: {
                print_value(pop(&vm.stack));
                printf("\n");
                return INTERPRET_OK;
            }
        }
    }
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


static void binary_op(Value (*op)(Value, Value)) {
    Value b = pop(&vm.stack);
    Value a = pop(&vm.stack);
    push(&vm.stack, op(a, b));
}
