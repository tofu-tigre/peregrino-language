#include <stdio.h>
#include "memory.h"
#include "debug.h"
#include "chunk.h"
#include "common.h"

int main(int argc, const char* argv[])
{
    Chunk chunk;
    init_chunk(&chunk);

    time_t t;
    srand((unsigned) time(&t));

    // Add constant to pool, save index of constant
    
    // Add opcode constant to chunk
    for(int i = 0x0; i < 512; i++) {
        write_constant(&chunk, i, (rand() % 100) + 1);
    }

    write_chunk(&chunk, OP_RETURN, 10);

    disassemble_chunk(&chunk, "test chunk");
    free_chunk(&chunk);
    return 0;
}