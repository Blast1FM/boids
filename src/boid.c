#include "raylib.h"
#include "raymath.h"
#include "boid.h"

#define LOCAL_FLOCK_DISTANCE 250
#define SEPARATION_RADIUS 100

Boid *createBoid(Vector2 position, Vector2 velocity, Boid *flock)
{
    Boid* boid = malloc(sizeof(Boid));

    boid->flock = flock;
    boid->position = position;
    boid->velocity = velocity;

    return boid;
}

Vector2 getSeparationVelocity(Boid* boid)
{
    float separationX = 0;
    float separationY = 0;
    int flockArrayLength = sizeof(boid->flock)/sizeof(Boid);
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

void updateBoid(Boid* boid)
{
    double deltaTime = GetFrameTime();
    Vector2 resultingVelocity = boid->velocity;

    Vector2 separationVelocity = getSeparationVelocity(boid);

    resultingVelocity = Vector2Add(resultingVelocity, separationVelocity);

    boid->velocity = resultingVelocity;
    boid->position.x = boid->position.x + boid->velocity.x * deltaTime;
    boid->position.y = boid->position.y + boid->velocity.y * deltaTime;
}

void drawBoid(Boid* boid)
{
    DrawCircle(boid->position.x, boid->position.y, 20, RED);
}