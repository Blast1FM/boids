#pragma once
#include <raylib.h>
#include "boidList.h"

#define BOID_CAP_PER_NODE 8

typedef struct QuadTree
{
    Rectangle bounds;
    bool divided;
    int depth;
    int boidsPresent;

    struct QuadTree* children[4];
    struct Boid* boids[BOID_CAP_PER_NODE];
} QuadTree;

void drawBounds(QuadTree* qtree);
// Recursive function to drill down to leaf nodes overlapping with the queried area, and append boids that are inside
void recursiveGetBoids(QuadTree* qtree, Rectangle queriedArea, BoidList* list);
// Function to get boids in a given rectangle, use when queried area overlaps with multiple nodes
BoidList getBoidsInRectangle(QuadTree* qtree, Rectangle targetRect);
QuadTree* initialiseTree(Rectangle bounds);
bool subdivide(QuadTree* qtree);
bool reassignBoidsToNewChildren(QuadTree* qtree);
bool insertBoidIntoTree(QuadTree* qtree, Boid* boid);
QuadTree* findLeafForPoint(QuadTree* qtree, Vector2 position);
bool tryInsertIntoNode(QuadTree* qtree, Boid* boid);
void freeTree(QuadTree* qtree);