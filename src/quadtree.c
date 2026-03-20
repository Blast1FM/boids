#include <stdlib.h>
#include <string.h>
#include <raylib.h>
#include "quadtree.h"
#include "boid.h"

// TODO regular insert boid

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
    for (int i = 0; i < qtree->boidsPresent; i++)
    {
        Boid* boid = qtree->boids[i];
        for (int j = 0; j < 4; j++)
        {
            if (CheckCollisionPointRec(boid->position, qtree->children[j]->bounds))
            {
                bool success = tryInsertIntoNode(boid, qtree->children[j]);
                
                // Not enough space in the node
                if (!success)
                {
                    subdivide(qtree->children[j]);
                    reassignBoidsToNewChildren(qtree->children[j]);
                    break;
                }

                // Remove the pointer from the array by shifting the end of the array back over it
                memcpy(&qtree->boids[i], &qtree->boids[i+1], BOID_CAP_PER_NODE-1 - i);
                // To account for shifting the array back one boid pointer
                i--;
                qtree->boidsPresent--;
                break;
            }
        }
            
    }
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
    qtree->boids[qtree->boidsPresent++] = boid;
    return true;
}