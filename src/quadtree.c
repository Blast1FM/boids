#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <raylib.h>
#include "quadtree.h"
#include "boid.h"
#include "boidList.h"

QuadTree* initialiseTree(Rectangle bounds, QuadTree* parent)
{
    QuadTree* tree = (QuadTree*)malloc(sizeof(QuadTree));
    memset(tree, 0, sizeof(QuadTree));

    tree->bounds = bounds;
    tree->isLeaf = true;
    tree->parent = parent;
    // Rest should be implicitly set to 0 due to memset
    return tree;
}

bool subdivide(QuadTree* qtree)
{
    if (!qtree->isLeaf)
    {
        return false;
    }

    float halfWidth = qtree->bounds.width / 2.0f;
    float halfHeight = qtree->bounds.height / 2.0f;

    Rectangle northWestBounds = (Rectangle){ x: qtree->bounds.x, y: qtree->bounds.y, width: halfWidth, height: halfHeight};
    Rectangle northEastBounds = (Rectangle){ x: qtree->bounds.x + halfWidth, y: qtree->bounds.y, width: qtree->bounds.width - halfWidth, height: halfHeight};
    Rectangle southEastBounds = (Rectangle){ x: qtree->bounds.x + halfWidth, y:qtree->bounds.y + halfHeight, width: qtree->bounds.width - halfWidth, height: qtree->bounds.height - halfHeight};
    Rectangle southWestBounds = (Rectangle){ x: qtree->bounds.x, y: qtree->bounds.y+halfHeight, width: halfWidth, height: qtree->bounds.height - halfHeight};

    QuadTree* northWest = initialiseTree(northWestBounds, qtree);
    QuadTree* northEast = initialiseTree(northEastBounds, qtree);
    QuadTree* southEast = initialiseTree(southEastBounds, qtree);
    QuadTree* southWest = initialiseTree(southWestBounds, qtree);

    qtree->children[0] = northWest;
    qtree->children[1] = northEast;
    qtree->children[2] = southEast;
    qtree->children[3] = southWest;

    northWest->depth = qtree->depth + 1;
    northEast->depth = qtree->depth + 1;
    southEast->depth = qtree->depth + 1;
    southWest->depth = qtree->depth + 1;

    qtree->isLeaf = false;

    bool reassigned = reassignBoidsToNewChildren(qtree);

    return reassigned;
}

bool reassignBoidsToNewChildren(QuadTree* qtree)
{
    if (qtree->isLeaf)
    {
        return false;
    }

    bool all_boids_reassigned_successfully = true;

    for (int i = 0; i < qtree->boidsPresent; i++)
    {
        Boid* boid = qtree->boids[i];
        bool current_boid_reassigned = false;
        for (int j = 0; j < 4; j++) 
        {
            if (PointInRectangle(boid->position, qtree->children[j]->bounds)) 
            {
                int inserted_result = insertBoidIntoTree(qtree->children[j], boid);
                
                if (inserted_result < 0)
                {
                    printf("Failed to insert boid with address %p when reassigning boids in node with address %p (error code %d)\n", (void*)boid, (void*)qtree, inserted_result);
                    boid->node = NULL; // Mark as orphan
                    all_boids_reassigned_successfully = false;
                } else {
                    current_boid_reassigned = true;
                }
                break;
            }
        }
        if (!current_boid_reassigned) {
            printf("Boid with address %x could not be reassigned to any child node and is now an orphan.\n", boid);
            // Mark as orphan if no child could take it.
            boid->node = NULL; 
            all_boids_reassigned_successfully = false;
        }
    }

    // Clear the boid pointer container and zero out present boids for newly non-leaf node
    qtree->boidsPresent = 0;
    memset(qtree->boids, 0, sizeof(Boid*) * BOID_CAP_PER_NODE);

    return all_boids_reassigned_successfully;
}

// Function to get boids in a given rectangle, use when queried area overlaps with multiple nodes
BoidList getBoidsInRectangle(QuadTree* qtree, Rectangle targetRect)
{
    BoidList list = {0};

    recursiveGetBoids(qtree, targetRect, &list);

    return list;
}

// Recursive function to drill down to leaf nodes overlapping with the queried area, and append boids that are inside to the passed list pointer
void recursiveGetBoids(QuadTree* qtree, Rectangle queriedArea, BoidList* list)
{
    // Drill down
    if (!qtree->isLeaf)
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
    for (int i = 0; i < qtree->boidsPresent; i++)
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

// Insert a boid into the quad tree, works with subtrees too
// Codes:
// -2 - Attempted to insert into a non leaf node
// -3 - Boid could not obtain node pointer for itself
int insertBoidIntoTree(QuadTree* qtree, Boid* boid)
{
    if (qtree == NULL || boid == NULL)
        return -4;

    QuadTree* leafNode = findLeafForPoint(qtree, boid->position);
    if (leafNode == NULL)
    {
        return -1;
    }

    int inserted = tryInsertIntoNode(leafNode, boid);

    if (inserted == -1)
    {
        if (!subdivide(leafNode))
        {
            return -1;
        }
        QuadTree* newLeaf = findLeafForPoint(leafNode, boid->position);
        if (newLeaf != NULL)
        {
            return insertBoidIntoTree(newLeaf, boid);
        }
        return -1;
    }

    if (inserted == -2)
    {
        printf("Attempted to insert boid %x into a non leaf node %x\n", boid, leafNode);
        return -2;
    }

    if (boid->node == NULL)
    {
        printf("Boid %x could not obtain ptr to its own node\n", boid);
        return -3;
    }
    
    return 0;
}

bool checkParentMergeEligibility(QuadTree* qtree)
{
    if (qtree->parent == NULL)
    {
        printf("Can not access parent of node %x, parent is NULL\n", qtree);
        return false;
    }

    if (!qtree->isLeaf)
    {
        printf("Can not merge non-leaf nodes\n");
        return false;
    }

    QuadTree* parent = qtree->parent;

    int boidCount = 0;

    for (int i = 0; i<4; i++)
    {
        boidCount += parent->children[i]->boidsPresent;
    }

    if (boidCount <= BOID_CAP_PER_NODE)
    {
        return true;
    }

    return false;
}

// Merges the given node into a single node
// Codes:
// -1 - Too many boids in children
// -2 - Failed to insert boid into the new node
int mergeQtreeNodes(QuadTree* parent)
{
    BoidList list = {0};
    for (int i = 0; i < 4; i++)
    {
        for(int j = 0; j < parent->children[i]->boidsPresent; j++)
        {
            blAppend(&list, parent->children[i]->boids[j]);
        }
    }

    // Too many boids in the children nodes, abort
    if (list.count > BOID_CAP_PER_NODE)
    {
        blDestroy(&list);
        return -1;
    }

    for (int i = 0; i < 4; i++)
    {
        freeTree(parent->children[i]);
        parent->children[i] = NULL; // Nullify child pointer after freeing
    }

    parent->isLeaf = true;
    parent->boidsPresent = 0; // Reset boidsPresent for the parent node

    bool all_boids_merged_successfully = true;
    for (int i = 0; i < list.count; i++)
    {
        int inserted = insertBoidIntoTree(parent, list.items[i]);

        if (inserted < 0)
        {
            printf("Failed to insert boid %x when merging node %x with code %d\n", list.items[i], parent, inserted);
            list.items[i]->node = NULL; // Mark as orphan
            all_boids_merged_successfully = false;
        }
    }

    blDestroy(&list);

    return all_boids_merged_successfully ? 0 : -2;
}

// Helper to check if a point is within a rectangle, including edges
bool PointInRectangle(Vector2 point, Rectangle rec)
{
    return (point.x >= rec.x && point.x <= rec.x + rec.width &&
            point.y >= rec.y && point.y <= rec.y + rec.height);
}

QuadTree* findLeafForPoint(QuadTree* qtree, Vector2 position)
{
    if (qtree == NULL)
        return NULL;

    if (!qtree->isLeaf)
    {
        for (int j = 0; j < 4; j++)
        {
            if (PointInRectangle(position, qtree->children[j]->bounds))
            {
                return findLeafForPoint(qtree->children[j], position);
            }
        }

        return NULL;
    }
    return qtree;
}

// Try to insert into a quad tree node that we know, no need to query
// Codes:
// -1 - Not enough space in node
// -2 - Node is not a leaf, can not take in boids
// -3 - Node is null
int tryInsertIntoNode(QuadTree* qtree, Boid* boid)
{
    if (qtree == NULL)
    {
        return -3;
    }

    if (qtree->boidsPresent >= BOID_CAP_PER_NODE)
    {
        // Not enough space
        return -1;
    }

    if (!qtree->isLeaf)
    {
        // Not a leaf, can not insert
        return -2;
    }

    // Place boid ptr, increment boids present
    // Possibly an out of bounds error here
    qtree->boids[qtree->boidsPresent++] = boid;
    boid->node = qtree;

    return 0;
}

void freeTree(QuadTree* qtree)
{
    if (qtree == NULL)
    {
        return;
    }

    if (!qtree->isLeaf)
    {
        for (int i = 0; i < 4; i++)
        {
            freeTree(qtree->children[i]);
        }
    }

    // Nullify boid->node pointers for boids in this quadtree node
    for (int i = 0; i < qtree->boidsPresent; i++)
    {
        if (qtree->boids[i] != NULL)
        {
            qtree->boids[i]->node = NULL;
        }
    }
    
    free(qtree);
}

void drawBounds(QuadTree* qtree)
{
    DrawRectangleLines(qtree->bounds.x, qtree->bounds.y, qtree->bounds.width, qtree->bounds.height, RED);
    if (!qtree->isLeaf)
    {
        for (int i = 0; i<4; i++)
        {
            drawBounds(qtree->children[i]);
        }
    }
}