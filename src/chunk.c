#include "chunk.h"


void init_chunk(Chunk *chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    /* Reallocate used instead of malloc because:
        1. Need to direct all memory allocation
           through the prior function (in memory.h)
           in order for garbage collector to
           have a count of memory in use.
    */
    chunk->code = NULL;
    chunk->lines = NULL;
    init_value_array(&chunk->constants);
}

void free_chunk(Chunk *chunk) {
    // Argument of old_size honestly doesn't matter...
    reallocate(chunk->code, sizeof(uint8_t) * chunk->capacity, 0);
    reallocate(chunk->lines, sizeof(int) * chunk->capacity, 0);
    free_value_array(&chunk->constants);
    init_chunk(chunk);
}

void write_chunk(Chunk *chunk, uint8_t byte, int line) {
    if (chunk->capacity < chunk->count + 1) {
        // Check if chunk data is NULL
        chunk->capacity = (chunk->capacity < INITIAL_CHUNK_SIZE) ?
            INITIAL_CHUNK_SIZE : chunk->capacity * CHUNK_GROWTH_FACTOR;
        
        chunk->code = reallocate(chunk->code, chunk->count * sizeof(uint8_t),
            chunk->capacity * sizeof(uint8_t));
        chunk->lines = reallocate(chunk->lines, chunk->count * sizeof(int),
            chunk->capacity * sizeof(int));
    }

    chunk->code[chunk->count] = byte;
    chunk->lines[chunk->count] = line;
    chunk->count += 1;
}

size_t add_constant(Chunk *chunk, Value value) {
    write_value_array(&chunk->constants, value);
    return chunk->constants.count - 1; // index of constant in values
}

void write_constant(Chunk *chunk, Value value, int line) {
    // Add value to chunk's constant array
    // Write instruction to chunk to handle value size (CONSTANT / CONSTANT_LONG)

    uint16_t index = add_constant(chunk, value);
    uint8_t left_bits = (index & 0xFF00) >> 8;
    if(index == 0x100) printf("INDEX IS 0x100\n");
    //printf("HEX: %x  LEFT: %x  RIGHT: %x\n", index, (index & 0xFF00) >> 8, index & 0x00FF);
    if(left_bits > 0) { // CONST_LONG
        if(index == 0x100) printf("INDEX IS 0x100\n");
        write_chunk(chunk, OP_CONSTANT_LONG, line);
        write_chunk(chunk, left_bits, line);
    } else {
        write_chunk(chunk, OP_CONSTANT, line);
    }
    write_chunk(chunk, index & 0x00FF, line);
}