#include <stdlib.h>
#include <stdio.h>
#include <raylib.h>
#include <raymath.h>
#include "boid.h"
#include "quadtree.h"
#include "boidParams.h"

Boid createBoid(Vector2 gameDimensions, BoidParams* params)
{
    Boid boid = {0};
    boid.position = (Vector2){GetRandomValue(-gameDimensions.x/2, gameDimensions.x/2),
            GetRandomValue(-gameDimensions.y/2, gameDimensions.y/2)};
    Vector2 velocity = (Vector2){x: GetRandomValue(0, params->maxSpeed), y: GetRandomValue(0, params->maxSpeed)};
    Vector2Normalize(velocity);
    boid.velocity = velocity;

    return boid;
}

Vector2 calculateWeightedSeparationVelocity(float* distance, Vector2* pos1, Vector2* pos2)
{
    Vector2 separationDirection = Vector2Normalize(Vector2Subtract(*pos1,*pos2));
    return (Vector2) Vector2Scale(separationDirection, 1.0f/(*distance));
}

int updateBoid(Boid* boid, BoidParams* params)
{
    if (boid->node == NULL)
    {
        return -1;
    }
    double deltaTime = GetFrameTime();

    // TODO ideally check for params being null here

    int encroachingNeighbourCount = 0;
    int alignmentNeighbourCount = 0;
    int cohesionNeighbourCount = 0;

    Vector2 separationVelocity = Vector2Zero();
    Vector2 alignmentVelocity = Vector2Zero();
    Vector2 localFlockCentre = Vector2Zero();
    Vector2 cohesionVelocity = Vector2Zero();

    for (int i = 0; i<boid->node->boidsPresent; i++)
    {
        if (boid == boid->node->boids[i]) continue;

        float distance = Vector2Distance(boid->position, boid->node->boids[i]->position);
        if(distance < params->separationRadius)
        {   
            // Separation logic
            separationVelocity = Vector2Add(separationVelocity,
            calculateWeightedSeparationVelocity(&distance, &(boid->position), &(boid->node->boids[i]->position)));
            encroachingNeighbourCount++;
        }
        if(distance < params->visibilityRadius)
        {
            // Alignment logic
            alignmentVelocity = Vector2Add(alignmentVelocity, boid->node->boids[i]->velocity);
            alignmentNeighbourCount++;
            // Cohesion logic
            localFlockCentre = Vector2Add(localFlockCentre, boid->node->boids[i]->position);
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

    if (boid->velocity.x == 0 && boid->velocity.y == 0)
    {
        printf("Boid with address %x updated with velocity 0\n", boid);
    }

    boid->position.x = boid->position.x + boid->velocity.x * deltaTime;
    boid->position.y = boid->position.y + boid->velocity.y * deltaTime;

    // TODO Add node transition check and logic
    return 0;
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
    if(boid->position.x >= halfWidth)
    {
        boid->position.x = -halfWidth+1;
    }
    if(boid->position.x <= -halfWidth)
    {
        boid->position.x = halfWidth-1;
    }
    if(boid->position.y >= halfHeight)
    {
        boid->position.y = -halfHeight+1;
    }
    if(boid->position.y <= -halfHeight)
    {
        boid->position.y = halfHeight-1;
    }
}