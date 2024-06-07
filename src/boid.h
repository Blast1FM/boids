#pragma once
#include "raylib.h"

typedef struct Boid
{
    Vector2 position;
    Vector2 velocity;
    Boid* flock;
}Boid;

Boid* createBoid(Vector2 position, Vector2 velocity, Boid* flock);
void updateBoid(Boid* boid);
void drawBoid(Boid* boid);

#include "boid.c"