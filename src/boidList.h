#pragma once
#include <stdlib.h>
#include "boid.h"

// Must be zero initialised if stack allocated
typedef struct BoidList
{
    Boid** items;
    size_t count;
    size_t capacity;
} BoidList;

int blAppend(BoidList* list, Boid* boid);

// Frees the internal buffer, used for stack allocated lists
int blDestroy(BoidList* list);
// Frees all memory used up by the list, used with heap allocated list
int blFree(BoidList* list);