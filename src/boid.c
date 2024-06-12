#include <stdlib.h>
#include "raylib.h"
#include "raymath.h"
#include "boid.h"

#define BOID_VISIBILITY_RADIUS 100
#define BOID_SEPARATION_RADIUS 30

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
        if(Vector2Distance(boid->position, boid->flock[i].position) < BOID_SEPARATION_RADIUS)
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
    if(neighbours == 0) return Vector2Zero();
    return (Vector2){alignmentX/(float)neighbours, alignmentY/(float)neighbours};
}

Vector2 getCohesionVelocity(Boid* boid, int flockArrayLength)
{
    float averageX = 0;
    float averageY = 0;
    float neighbours = 0;

    for(int i = 0; i<flockArrayLength; i++)
    {
        if(boid==&boid->flock[i]) continue;
        if(Vector2Distance(boid->position, boid->flock[i].position)<BOID_VISIBILITY_RADIUS)
        {
            neighbours+=1;
            averageX += boid->flock[i].position.x;
            averageY += boid->flock[i].position.y;
        }
    }
    if (neighbours == 0) return Vector2Zero();
    return (Vector2){averageX-boid->position.x, averageY-boid->position.y};
}

void updateBoid(Boid* boid, int flockArrayLength)
{
    double deltaTime = GetFrameTime();
    float separationFactor = 1.0f;
    float alignmentFactor = 0.5f;
    float cohesionFactor = 0.5f;

    int encroachingNeighbourCount = 0;
    int visibleNeighbourCount = 0;

    Vector2 finalVelocity = boid->velocity;

    Vector2 separationVelocity = Vector2Zero();
    Vector2 alignmentVelocity = Vector2Zero();
    Vector2 cohesionVelocity = Vector2Zero();
    
    for(int i = 0; i<flockArrayLength; i++)
    {
        if(boid == &boid->flock[i]) continue;
        float distance = Vector2Distance(boid->position, boid->flock[i].position);
        if(distance < BOID_SEPARATION_RADIUS)
        {   
            Vector2 separationDirection = Vector2Normalize((boid->position, boid->flock[i].position));
            Vector2 weightedSeparationVelocity = Vector2Scale(separationDirection, 1.0f/distance);
            separationVelocity = Vector2Add(separationVelocity, weightedSeparationVelocity);
            encroachingNeighbourCount++;
        }
        if(distance < BOID_VISIBILITY_RADIUS)
        {
            //TODO alignment and cohesion logic
        }
    }

    if(encroachingNeighbourCount > 0)
    {
        separationVelocity = Vector2Scale(separationVelocity, 1/encroachingNeighbourCount);
        separationVelocity = Vector2Scale(separationVelocity, separationFactor);
    }

    boid->velocity = Vector2Add(boid->velocity, separationVelocity);

    boid->position.x = boid->position.x + boid->velocity.x * deltaTime;
    boid->position.y = boid->position.y + boid->velocity.y * deltaTime;
}

void drawBoid(Boid* boid)
{
    DrawCircleV(boid->position, 5, RED);
    DrawCircleLinesV(boid->position,BOID_VISIBILITY_RADIUS, BLUE);
    DrawCircleLinesV(boid->position,BOID_SEPARATION_RADIUS, GREEN);
}