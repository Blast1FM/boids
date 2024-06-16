#pragma once

typedef struct BoidParams
{

    int separationRadius;
    int visibilityRadius;

    float separationFactor;
    float alignmentFactor;
    float cohesionFactor;

    float maxSpeed;

    int flockArrayLength;

}BoidParams;