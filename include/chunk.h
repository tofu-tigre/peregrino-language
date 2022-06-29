#ifndef CHUNK_H
#define CHUNK_H

#include "common.h"
#include "memory.h"
#include "value.h"

typedef enum {
    OP_RETURN,
    OP_CONSTANT,
    OP_CONSTANT_LONG,
    OP_NEGATE,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE
} OpCode;

typedef struct {
    int count;
    int capacity;
    int *lines;
    int line_count;
    int line_capacity;
    uint8_t *code;
    ValueArray constants;
} Chunk;

void init_chunk(Chunk *chunk);
void write_chunk(Chunk *chunk, uint8_t byte, int line);
void free_chunk(Chunk *chunk);
int get_line(Chunk *chunk, int offset);
void write_constant(Chunk *chunk, Value value, int line);
size_t add_constant(Chunk *chunk, Value value);

#endif