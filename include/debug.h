#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include "chunk.h"
#include "value.h"

void disassemble_chunk(Chunk *chunk, const char *name);
int disassemble_instruction(Chunk *chunk, int offset);

#endif