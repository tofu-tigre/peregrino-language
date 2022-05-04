#include <stdio.h>
#include "memory.h"
#include "debug.h"
#include "chunk.h"

int main(int argc, const char* argv[])
{
    Chunk chunk;
    init_chunk(&chunk);

    // Add constant to pool, save index of constant
    uint8_t constant = add_constant(&chunk, 1.23);
    uint8_t constant2 = add_constant(&chunk, 543.4);
    uint8_t constant3 = add_constant(&chunk, 23.4);
    uint8_t constant4 = add_constant(&chunk, 87.832);
    
    // Add opcode constant to chunk
    write_chunk(&chunk, OP_CONSTANT, 1);
    write_chunk(&chunk, constant, 1);

    write_chunk(&chunk, OP_CONSTANT, 2);
    write_chunk(&chunk, constant3, 2);

    write_chunk(&chunk, OP_CONSTANT, 3);
    write_chunk(&chunk, constant2, 3);

    write_chunk(&chunk, OP_CONSTANT, 4);
    write_chunk(&chunk, constant4, 4);

    write_chunk(&chunk, OP_RETURN, 5);

    disassemble_chunk(&chunk, "test chunk");
    free_chunk(&chunk);
    return 0;
}