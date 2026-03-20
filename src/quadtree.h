#pragma once
#include <raylib.h>
#include "boid.h"

#define BOID_CAP_PER_NODE 8

typedef struct QuadTree
{
    Rectangle bounds;
    bool divided;
    int depth;
    int boidsPresent;

    QuadTree* children[4];

    Boid* boids[BOID_CAP_PER_NODE];
} QuadTree;
