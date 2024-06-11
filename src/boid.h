#pragma once
#include "raylib.h"

typedef struct Boid Boid;

typedef struct Boid
{
    Vector2 position;
    Vector2 velocity;
    Boid* flock;
}Boid;

Boid* createBoid(Vector2 position, Vector2 velocity, Boid* flock);
Vector2 getSeparationVelocity(Boid* boid, int flockArrayLength);
Vector2 getAverageNeighbourVelocity(Boid* boid, int flockArrayLength);
Vector2 getCohesionVelocity(Boid* boid, int flockArrayLength);
void updateBoid(Boid* boid, int flockArrayLength);
void drawBoid(Boid* boid);