#include <stdio.h>
#include "memory.h"
#include "debug.h"
#include "chunk.h"

int main(int argc, const char* argv[])
{
    Chunk chunk;
    init_chunk(&chunk);

    // Add constant to pool, save index of constant
    
    // Add opcode constant to chunk
    for(int i = 0x0; i < 0x10F; i++) {
        write_constant(&chunk, i, i + 1);
    }

    write_chunk(&chunk, OP_RETURN, 517);

    disassemble_chunk(&chunk, "test chunk");
    free_chunk(&chunk);
    return 0;
}