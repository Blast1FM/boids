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

void drawBounds(QuadTree* qtree);
QuadTree* initialiseTree(Rectangle bounds);
bool subdivide(QuadTree* qtree);
bool reassignBoidsToNewChildren(QuadTree* qtree);
bool insertBoidIntoTree(QuadTree* qtree, Boid* boid);
QuadTree* findLeafForPoint(QuadTree* qtree, Vector2 position);
bool tryInsertIntoNode(QuadTree* qtree, Boid* boid);