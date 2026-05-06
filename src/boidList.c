#include <stdlib.h>
#include "boidList.h"
#include "boid.h"

// Error code -1 : failed to reallocate
int blAppend(BoidList* list, Boid* boid)
{
    if (list->count >= list->capacity)
    {
        if (list->capacity == 0) list->capacity = 128;
        else list->capacity *=2;

        void* tmp = realloc(list->items, list->capacity * sizeof(*list->items));

        if (!tmp) {
            return -1;
        }   
        list->items = tmp;
    }

    list->items[list->count++] = boid;
    return 0;
}

// Frees the internal buffer, used for stack allocated lists
int blDestroy(BoidList* list)
{
    if (list != NULL) {
        free(list->items);
        list->items = NULL;
        return 0;
    }

    return -1;
}

// Frees all memory used up by the list, used with heap allocated list
int blFree(BoidList* list)
{
    if (list != NULL) {
        free(list->items);
        free(list);
        return 0;
    }

    return -1;
}