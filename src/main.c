#include <stdio.h>
#include "memory.h"
#include "debug.h"
#include "chunk.h"
#include "common.h"
#include "vm.h"

int main(int argc, const char* argv[])
{
    time_t t;
    srand((unsigned) time(&t));

    VM vm;
    Chunk chunk;
    init_vm(&vm);
    init_chunk(&chunk);

    for(int i = 0; i < 257; i++) {
        write_constant(&chunk, i, i);
        write_constant(&chunk, i, i);
        write_chunk(&chunk, OP_ADD, 1);
    }

    

    write_chunk(&chunk, OP_RETURN, 2);

    interpret(&vm, &chunk);
    free_vm(&vm);
    return 0;
}