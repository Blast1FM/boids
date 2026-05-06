#pragma once
#include <raylib.h>
#include "boidParams.h"

struct QuadTree;

typedef struct Boid
{
    Vector2 position;
    Vector2 velocity;
    struct QuadTree* node;
}Boid;

Boid createBoid(Vector2 gameDimensions, BoidParams* params);
int updateBoid(Boid* boid, BoidParams* params);
void drawBoid(Boid* boid, BoidParams* params, bool drawRadii);
void yeetBoidBackIntoVisibleArea(Boid* boid, int haldWidth, int halfHeight);