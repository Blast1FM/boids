#include <stdlib.h>
#include "raylib.h"
#include "raymath.h"
#include "boid.h"
#include "boidParams.h"

Boid *createBoid(Vector2 position, Vector2 velocity, Boid *flock)
{
    Boid* boid = malloc(sizeof(Boid));

    boid->flock = flock;
    boid->position = position;
    boid->velocity = velocity;

    return boid;
}

Vector2 calculateWeightedSeparationVelocity(float* distance, Vector2* pos1, Vector2* pos2)
{
    Vector2 separationDirection = Vector2Normalize(Vector2Subtract(*pos1,*pos2));
    return (Vector2) Vector2Scale(separationDirection, 1.0f/(*distance));
}

void updateBoid(Boid* boid, BoidParams* params)
{
    double deltaTime = GetFrameTime();

    // TODO ideally check for params being null here

    int encroachingNeighbourCount = 0;
    int alignmentNeighbourCount = 0;
    int cohesionNeighbourCount = 0;

    Vector2 separationVelocity = Vector2Zero();
    Vector2 alignmentVelocity = Vector2Zero();
    Vector2 localFlockCentre = Vector2Zero();
    Vector2 cohesionVelocity = Vector2Zero();
    
    for(int i = 0; i<params->flockArrayLength; i++)
    {
        if(boid == &boid->flock[i]) continue;
        float distance = Vector2Distance(boid->position, boid->flock[i].position);
        if(distance < params->separationRadius)
        {   
            // Separation logic
            separationVelocity = Vector2Add(separationVelocity,
            calculateWeightedSeparationVelocity(&distance, &(boid->position), &(boid->flock[i].position)));
            encroachingNeighbourCount++;
        }
        if(distance < params->visibilityRadius)
        {
            // Alignment logic
            alignmentVelocity = Vector2Add(alignmentVelocity, boid->flock[i].velocity);
            alignmentNeighbourCount++;
            // Cohesion logic
            localFlockCentre = Vector2Add(localFlockCentre, boid->flock[i].position);
            cohesionNeighbourCount++;
        }
    }

    if(encroachingNeighbourCount > 0)
    {
        separationVelocity = Vector2Scale(separationVelocity, 1/encroachingNeighbourCount);
        separationVelocity = Vector2Scale(separationVelocity, params->separationFactor);
    }

    if(alignmentNeighbourCount > 0)
    {
        alignmentVelocity = Vector2Scale(alignmentVelocity, 1/alignmentNeighbourCount);
        alignmentVelocity = Vector2Scale(alignmentVelocity, params->alignmentFactor);
    }

    if(cohesionNeighbourCount > 0)
    {
        localFlockCentre = Vector2Scale(localFlockCentre, 1/cohesionNeighbourCount);
        Vector2 localFlockCentreDirection = Vector2Subtract(localFlockCentre, boid->position);
        localFlockCentreDirection = Vector2Normalize(localFlockCentreDirection);
        cohesionVelocity = Vector2Scale(localFlockCentreDirection, params->cohesionFactor);
    }

    boid->velocity = Vector2Add(boid->velocity, separationVelocity);
    boid->velocity = Vector2Add(boid->velocity, alignmentVelocity);
    boid->velocity = Vector2Add(boid->velocity, cohesionVelocity);

    // This is a crutch, TODO devise a more sophisticated speedlimit
    boid->velocity = Vector2ClampValue(boid->velocity, 0, params->maxSpeed);

    boid->position.x = boid->position.x + boid->velocity.x * deltaTime;
    boid->position.y = boid->position.y + boid->velocity.y * deltaTime;
}

void drawBoid(Boid* boid, BoidParams* params, bool drawRadii)
{
    DrawCircleV(boid->position, 5, RED);
    if(drawRadii)
    {
        DrawCircleLinesV(boid->position,params->visibilityRadius, BLUE);
        DrawCircleLinesV(boid->position,params->separationRadius, GREEN);
    }
}

// Why is this in worldspace after all? I thought I was working with screenspace all this time
// but it doesn't work that way
// TODO maybe think of a more sophisticated algo for this cycle hog
void yeetBoidBackIntoVisibleArea(Boid* boid, int halfWidth, int halfHeight)
{
    if(boid->position.x > halfWidth)
    {
        boid->position.x = -halfWidth;
    }
    if(boid->position.x < -halfWidth)
    {
        boid->position.x = halfWidth;
    }
    if(boid->position.y > halfHeight)
    {
        boid->position.y = -halfHeight;
    }
    if(boid->position.y < -halfHeight)
    {
        boid->position.y = halfHeight;
    }
}