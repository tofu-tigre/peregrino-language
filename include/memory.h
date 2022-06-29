#ifndef MEMORY_H
#define MEMORY_H

#include <stdlib.h>
#include "common.h"

#define CHUNK_GROWTH_FACTOR 2
#define INITIAL_CHUNK_SIZE 8
#define INITIAL_CHUNK_LINE_SIZE 8

void *reallocate(void *pointer, size_t old_size, size_t new_size);

#endif