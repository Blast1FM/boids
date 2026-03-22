#include <stdlib.h>
#include <string.h>
#include <raylib.h>
#include "quadtree.h"
#include "boid.h"


bool subdivide(QuadTree* qtree)
{
    if (qtree->divided)
    {
        return false;
    }

    QuadTree* northWest = malloc(sizeof(QuadTree));
    QuadTree* northEast = malloc(sizeof(QuadTree));
    QuadTree* southEast = malloc(sizeof(QuadTree));
    QuadTree* southWest = malloc(sizeof(QuadTree));

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

}

// TODO check logic, reassignment may not work correctly
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
                // Do not increment i – the next element has shifted into position i
                break;
            }
        }
            
    }
}

void insertBoidIntoTree(QuadTree* qtree, Boid* boid)
{   
    QuadTree* leafNode = findLeafForPoint(qtree, boid->position);

    bool inserted = tryInsertIntoNode(leafNode, boid);

    if (!inserted)
    {
        subdivide(leafNode);
        QuadTree* newLeaf = findLeafForPoint(leafNode, boid->position);
        insertBoidIntoTree(newLeaf, boid);
    }
}

QuadTree* findLeafForPoint(QuadTree* qtree, Vector2 position)
{
    if (qtree->divided)
    {
        for (int j = 0; j < 4; j++)
        {
            if (CheckCollisionPointRec(position, qtree->children[j]->bounds))
            {
                return findLeafForPoint(qtree->children[j], position);
            }
        }
    }

    return qtree;
}

// Try to insert into a quad tree node that we know, no need to query
bool tryInsertIntoNode(QuadTree* qtree, Boid* boid)
{
    if (qtree->boidsPresent >= BOID_CAP_PER_NODE)
    {
        // Not enough space
        return false;
    }

    // Place boid ptr, increment boids present
    // Possibly an out of bounds error here
    qtree->boids[qtree->boidsPresent++] = boid;
    return true;
}