#include "debug.h"


static int simple_instruction(const char *name, int offset) {
    printf("%s\n", name);
    return offset + 1;
}

static int constant_instruction(const char *name, Chunk *chunk, int offset) {
    uint8_t constant_index = chunk->code[offset + 1];
    printf("%-16s Index: %4d Value: ", name, constant_index);
    print_value(chunk->constants.values[constant_index]);
    printf("\n");
    return offset + 2;
}

static int constant_long_instruction(const char *name, Chunk *chunk, int offset) {
    uint16_t constant_index = (chunk->code[offset + 1] << 8) | chunk->code[offset + 2];
    printf("%-16s Index: %4d Value: ", name, constant_index);
    print_value(chunk->constants.values[constant_index]);
    printf("\n");
    return offset + 3;
}

void disassemble_chunk(Chunk *chunk, const char *name) {
    printf("== %s ==\n", name);

    for (int offset = 0; offset < chunk->count;) {
        // NOTE: disassemble_instruction increments offset.
        offset = disassemble_instruction(chunk, offset);
    }
}

int disassemble_instruction(Chunk *chunk, int offset) {
    printf("%04d ", offset);
    if (offset > 0 &&
        get_line(chunk, offset) == get_line(chunk, offset - 1)) {
            printf(" | ");
        } else {
            printf("Line: %4d ", get_line(chunk, offset));
        }

    uint8_t instruction = chunk->code[offset];
    switch (instruction) {
        case OP_RETURN:
            return simple_instruction("OP_RETURN", offset);
        case OP_CONSTANT:
            return constant_instruction("OP_CONSTANT", chunk, offset);
        case OP_CONSTANT_LONG:
            return constant_long_instruction("OP_CONSTANT_LONG", chunk, offset);
        case OP_NEGATE:
            return simple_instruction("OP_NEGATE", offset);
        case OP_ADD:
            return simple_instruction("OP_ADD", offset);
        case OP_SUBTRACT:
            return simple_instruction("OP_SUBTRACT", offset);
        case OP_MULTIPLY:
            return simple_instruction("OP_MULTIPLY", offset);
        case OP_DIVIDE:
            return simple_instruction("OP_DIVIDE", offset);
        default:
            printf("Unknown opcode %d\n", instruction);
            return offset + 1;
    }
}
