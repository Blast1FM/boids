#pragma once
#include <raylib.h>
#include "boid.h"

#define BOID_CAP_PER_NODE 8

typedef struct QuadTree
{
    Rectangle bounds;
    bool divided;
    int depth;
    int boidsPresent;

    struct QuadTree* children[4];

    Boid* boids[BOID_CAP_PER_NODE];
} QuadTree;

bool subdivide(QuadTree* qtree);
bool reassignBoidsToNewChildren(QuadTree* qtree);
void insertBoidIntoTree(QuadTree* qtree, Boid* boid);
QuadTree* findLeafForPoint(QuadTree* qtree, Vector2 position);
bool tryInsertIntoNode(QuadTree* qtree, Boid* boid);