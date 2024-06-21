#pragma once
#include "raylib.h"
#include "boidParams.h"

typedef struct Boid Boid;

typedef struct Boid
{
    Vector2 position;
    Vector2 velocity;
    Boid* flock;
}Boid;

Boid* createBoid(Vector2 position, Vector2 velocity, Boid* flock);
void updateBoid(Boid* boid, BoidParams* params);
void drawBoid(Boid* boid, BoidParams* params, bool drawRadii);