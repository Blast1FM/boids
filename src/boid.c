#include <stdlib.h>
#include "raylib.h"
#include "raymath.h"
#include "boid.h"

#define BOID_VISIBILITY_RADIUS 100
#define SEPARATION_RADIUS 30

Boid *createBoid(Vector2 position, Vector2 velocity, Boid *flock)
{
    Boid* boid = malloc(sizeof(Boid));

    boid->flock = flock;
    boid->position = position;
    boid->velocity = velocity;

    return boid;
}

Vector2 getSeparationVelocity(Boid* boid, int flockArrayLength)
{
    float separationX = 0;
    float separationY = 0;
    for (int i = 0; i<flockArrayLength; i++)
    {
        if(boid == &boid->flock[i]) continue;
        if(Vector2Distance(boid->position, boid->flock[i].position) < SEPARATION_RADIUS)
        {
            separationX += boid->position.x - boid->flock[i].position.x;
            separationY += boid->position.y - boid->flock[i].position.y;
        }
    }
    return (Vector2){separationX, separationY};
}

Vector2 getAverageNeighbourVelocity(Boid* boid, int flockArrayLength)
{
    float alignmentX = 0;
    float alignmentY = 0;
    int neighbours = 0;

    for(int i = 0; i<flockArrayLength; i++)
    {
        if(boid == &boid->flock[i]) continue;

        if(Vector2Distance(boid->position, boid->flock[i].position)<BOID_VISIBILITY_RADIUS)
        {
            neighbours += 1;
            alignmentX += boid->flock[i].velocity.x;
            alignmentY += boid->flock[i].velocity.y;
        }
    }

    return (Vector2){alignmentX/(float)neighbours, alignmentY/(float)neighbours};
}

Vector2 getCohesionVelocity(Boid* boid, int flockArrayLength)
{
    return Vector2Zero();
}

void updateBoid(Boid* boid, int flockArrayLength)
{
    double deltaTime = GetFrameTime();
    float avoidFactor = 0.5f;
    float alignmentFactor = 1.0f;
    Vector2 resultingVelocity = boid->velocity;
    
    Vector2 separationVelocity = getSeparationVelocity(boid, flockArrayLength);
    Vector2 alignmentVelocity = getAverageNeighbourVelocity(boid, flockArrayLength);


    Vector2 resultingVelocitySeparation = Vector2Add(resultingVelocity, Vector2Scale(separationVelocity, avoidFactor));
    Vector2 resultingVelocityAlignment = Vector2Scale(Vector2Subtract(alignmentVelocity, (Vector2){boid->velocity.x,boid->velocity.y}),alignmentFactor);

    resultingVelocity = Vector2Add(resultingVelocitySeparation, resultingVelocityAlignment);

    boid->velocity = Vector2Add(boid->velocity, resultingVelocity);
    boid->position.x = boid->position.x + boid->velocity.x * deltaTime;
    boid->position.y = boid->position.y + boid->velocity.y * deltaTime;
}

void drawBoid(Boid* boid)
{
    DrawCircle(boid->position.x, boid->position.y, 5, RED);
}