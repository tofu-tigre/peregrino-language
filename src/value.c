#include "value.h"


void init_value_array(ValueArray *array) {
    array->count = 0;
    array->capacity = 0;
    /* Reallocate used instead of malloc because:
        1. Need to direct all memory allocation
           through the prior function (in memory.h)
           in order for garbage collector to
           have a count of memory in use.
    */
    array->values = NULL;
}

void free_value_array(ValueArray *array) {
    // Argument of old_size honestly doesn't matter...
    reallocate(array->values, sizeof(Value) * array->capacity, 0);
    init_value_array(array);
}

void write_value_array(ValueArray *array, Value value) {
    assert(array->count <= MAX_REPRESENTABLE_CONST);

    if (array->capacity < array->count + 1) {
        // Check if chunk data is NULL
        array->capacity = (array->capacity < INITIAL_CHUNK_SIZE) ?
            INITIAL_CHUNK_SIZE : array->capacity * CHUNK_GROWTH_FACTOR;
        
        array->values = reallocate(array->values, array->count * sizeof(Value),
            array->capacity * sizeof(Value));
    }

    array->values[array->count] = value;
    array->count += 1;
}

void print_value(Value value) {
    printf("%g", value);
}