#include "chunk.h"


void init_chunk(Chunk *chunk) {
    chunk->count = 0;
    chunk->capacity = INITIAL_CHUNK_SIZE;
    /* Reallocate used instead of malloc because:
        1. Need to direct all memory allocation
           through the prior function (in memory.h)
           in order for garbage collector to
           have a count of memory in use.
    */
    chunk->code = reallocate(chunk->code, 0, INITIAL_CHUNK_SIZE * sizeof(uint8_t));
    chunk->lines = reallocate(chunk->lines, 0, INITIAL_CHUNK_LINE_SIZE * sizeof(int));
    chunk->lines[0] = -1;
    chunk->lines[1] = -1;
    chunk->line_count = 2;
    chunk->line_capacity = INITIAL_CHUNK_LINE_SIZE;
    init_value_array(&chunk->constants);
}

void free_chunk(Chunk *chunk) {
    // Argument of old_size honestly doesn't matter...
    chunk->code = reallocate(chunk->code, sizeof(uint8_t) * chunk->capacity, 0);
    chunk->lines = reallocate(chunk->lines, sizeof(int) * chunk->line_capacity, 0);
    free_value_array(&chunk->constants);
    init_chunk(chunk);
}

static void add_line(Chunk *chunk, int line) {
    // Check if there is enough space for new element
    //printf("LINE CAPACITY: %d  LINE COUNT: %d\n", chunk->line_capacity, chunk->line_count);
    if (chunk->line_capacity < chunk->line_count + 1) {
        // Check if chunk data is NULL
        chunk->line_capacity = chunk->line_capacity * CHUNK_GROWTH_FACTOR;
        //printf("NEW LINE CAPACITY: %d\n", chunk->line_capacity);
        
        chunk->lines = reallocate(chunk->lines, chunk->line_count * sizeof(int),
            chunk->line_capacity * sizeof(int));
    }
    // Check if previous line is the same as new line
    int curr_line_run_el = chunk->lines[chunk->line_count - 1];
    
    //printf("CURRENT RUN ELEMENT: %d W/ %d REPEATS\n", curr_line_run_el, chunk->lines[chunk->line_count - 2]);
    if (curr_line_run_el != line) {
        chunk->lines[chunk->line_count] = 1;
        chunk->lines[chunk->line_count + 1] = line;
        chunk->line_count += 2;
    } else {
        chunk->lines[chunk->line_count - 2] += 1;
    }


}


int get_line(Chunk *chunk, int offset) {
    int index = 2;


    // Given offset, eat through chunk lines until offset == 0
    //printf("OFFSET: %d\n", offset);

    while(offset > 0) {
        offset -= chunk->lines[index];
        //printf("OFFSET: %d\n", offset);
        if(offset < 0) return chunk->lines[index + 1];
        index += 2;
        if(offset == 0) return chunk->lines[index + 1];
    }
    return chunk->lines[index + 1];
}


void write_chunk(Chunk *chunk, uint8_t byte, int line) {
    if (chunk->capacity < chunk->count + 1) {
        // Check if chunk data is NULL
        chunk->capacity = chunk->capacity * CHUNK_GROWTH_FACTOR;
        
        chunk->code = reallocate(chunk->code, chunk->count * sizeof(uint8_t),
            chunk->capacity * sizeof(uint8_t));
    }

    chunk->code[chunk->count] = byte;
    add_line(chunk, line);
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
    //printf("HEX: %x  LEFT: %x  RIGHT: %x\n", index, (index & 0xFF00) >> 8, index & 0x00FF);
    if(left_bits > 0) { // CONST_LONG
        write_chunk(chunk, OP_CONSTANT_LONG, line);
        write_chunk(chunk, left_bits, line);
    } else {
        write_chunk(chunk, OP_CONSTANT, line);
    }
    write_chunk(chunk, index & 0x00FF, line);
}