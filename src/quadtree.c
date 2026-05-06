#include <stdlib.h>
#include <string.h>
#include <raylib.h>
#include "quadtree.h"
#include "boid.h"
#include "boidList.h"

QuadTree* initialiseTree(Rectangle bounds)
{
    QuadTree* tree = (QuadTree*)malloc(sizeof(QuadTree));
    memset(tree, 0, sizeof(QuadTree));

    tree->bounds = bounds;

    // Rest should be implicitly set to 0 due to memset
    return tree;
}

bool subdivide(QuadTree* qtree)
{
    if (qtree->divided)
    {
        return false;
    }

    QuadTree* northWest = (QuadTree*)malloc(sizeof(QuadTree));
    QuadTree* northEast = (QuadTree*)malloc(sizeof(QuadTree));
    QuadTree* southEast = (QuadTree*)malloc(sizeof(QuadTree));
    QuadTree* southWest = (QuadTree*)malloc(sizeof(QuadTree));

    memset(northWest, 0, sizeof(QuadTree));
    memset(northEast, 0, sizeof(QuadTree));
    memset(southEast, 0, sizeof(QuadTree));
    memset(southWest, 0, sizeof(QuadTree));

    int halfWidth = qtree->bounds.width / 2;
    int halfHeight = qtree->bounds.height / 2;

    northWest->bounds = (Rectangle){ x: qtree->bounds.x, y: qtree->bounds.y, width: halfWidth, height: halfHeight};
    northEast->bounds = (Rectangle){ x: qtree->bounds.x + halfWidth, y: qtree->bounds.y, width: halfWidth, height: halfHeight};

    southEast->bounds = (Rectangle){ x: qtree->bounds.x + halfWidth, y:qtree->bounds.y + halfHeight, width: halfWidth, height: halfHeight};
    southWest->bounds = (Rectangle){ x: qtree->bounds.x, y: qtree->bounds.y+halfHeight, width: halfWidth, height: halfHeight};

    qtree->children[0] = northWest;
    qtree->children[1] = northEast;
    qtree->children[2] = southEast;
    qtree->children[3] = southWest;

    northWest->depth = qtree->depth + 1;
    northEast->depth = qtree->depth + 1;
    southEast->depth = qtree->depth + 1;
    southWest->depth = qtree->depth + 1;

    qtree->divided = true;

    bool reassigned = reassignBoidsToNewChildren(qtree);

    return reassigned;
}

bool reassignBoidsToNewChildren(QuadTree* qtree)
{
    if (!qtree->divided)
    {
        return false;
    }

    for (int i = 0; i < qtree->boidsPresent; i++)
    {
        Boid* boid = qtree->boids[i];
        for (int j = 0; j < 4; j++) 
        {
            if (CheckCollisionPointRec(boid->position, qtree->children[j]->bounds)) 
            {
                insertBoidIntoTree(qtree->children[j], boid);

                // Remove pointer from parent's array (shift remaining elements)
                size_t elementsToMove = qtree->boidsPresent - i - 1;
                if (elementsToMove > 0) 
                {
                    memmove(&qtree->boids[i], &qtree->boids[i + 1],
                            elementsToMove * sizeof(Boid*));
                }
                qtree->boidsPresent--;
                i--;
                // Do not increment i – the next element has shifted into position i
                break;
            }
        }
            
    }

    return true;
}

// Function to get boids in a given rectangle, use when queried area overlaps with multiple nodes
BoidList getBoidsInRectangle(QuadTree* qtree, Rectangle targetRect)
{
    BoidList list = {0};

    recursiveGetBoids(qtree, targetRect, &list);

    return list;
}

// Recursive function to drill down to leaf nodes overlapping with the queried area, and append boids that are inside
void recursiveGetBoids(QuadTree* qtree, Rectangle queriedArea, BoidList* list)
{
    // Drill down
    if (qtree->divided)
    {
        for (int i = 0; i < 4; i++)
        {
            if (CheckCollisionRecs(qtree->children[i]->bounds, queriedArea))
            {
                recursiveGetBoids(qtree->children[i], queriedArea, list);
            }
        }
    } else
    // Leaf node - should have an overlap already due to how drill down works 
    {
        for (int j = 0; j < qtree->boidsPresent; j++)
        {
            if (CheckCollisionPointRec(qtree->boids[j]->position, queriedArea))
            {
                blAppend(list, qtree->boids[j]);
            }
        }
    }
}

bool removeBoidFromNode(QuadTree* qtree, Boid* boid)
{
    for (int i = 0; i < BOID_CAP_PER_NODE; i++)
    {
        if (qtree->boids[i] == boid)
        {
            size_t elementsToMove = qtree->boidsPresent - i - 1;
            if (elementsToMove > 0) 
            {
                memmove(&qtree->boids[i], &qtree->boids[i + 1],
                        elementsToMove * sizeof(Boid*));
            }
            qtree->boidsPresent--;
            boid->node = NULL;

            return true;
        }
    }

    return false;
}

bool insertBoidIntoTree(QuadTree* qtree, Boid* boid)
{
    if (qtree == NULL || boid == NULL)
        return false;

    QuadTree* leafNode = findLeafForPoint(qtree, boid->position);
    if (leafNode == NULL)
    {
        // Point lies outside every leaf; insertion is impossible.
        return false;
    }

    bool inserted = tryInsertIntoNode(leafNode, boid);

    if (!inserted)
    {
        subdivide(leafNode);
        QuadTree* newLeaf = findLeafForPoint(leafNode, boid->position);
        if (newLeaf != NULL)
        {
            return insertBoidIntoTree(newLeaf, boid);
        }
    }
    
    boid->node = leafNode;
    return true;
}

QuadTree* findLeafForPoint(QuadTree* qtree, Vector2 position)
{
    if (qtree == NULL)
        return NULL;

    if (qtree->divided)
    {
        for (int j = 0; j < 4; j++)
        {
            if (CheckCollisionPointRec(position, qtree->children[j]->bounds))
            {
                return findLeafForPoint(qtree->children[j], position);
            }
        }

        return NULL;
    }

    return qtree;
}

// Try to insert into a quad tree node that we know, no need to query
bool tryInsertIntoNode(QuadTree* qtree, Boid* boid)
{
    if (qtree->boidsPresent >= BOID_CAP_PER_NODE || qtree->divided)
    {
        // Not enough space or this isnt a leaf node
        return false;
    }

    // Place boid ptr, increment boids present
    // Possibly an out of bounds error here
    qtree->boids[qtree->boidsPresent++] = boid;
    return true;
}

void freeTree(QuadTree* qtree)
{
    if (qtree == NULL)
    {
        return;
    }

    if (qtree->divided)
    {
        for (int i = 0; i < 4; i++)
        {
            freeTree(qtree->children[i]);
        }
    }
    free(qtree);
}

void drawBounds(QuadTree* qtree)
{
    DrawRectangleLines(qtree->bounds.x, qtree->bounds.y, qtree->bounds.width, qtree->bounds.height, RED);
    if (qtree->divided)
    {
        for (int i = 0; i<4; i++)
        {
            drawBounds(qtree->children[i]);
        }
    }
}