#include <stdlib.h>
#include <raylib.h>
#include <raymath.h>
#include "boidParams.h"
#include "boidList.h"
#include "quadtree.h"
#include "boid.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define BOID_COUNT 256

typedef struct 
{
    int ScreenWidth;
    int ScreenHeight;
} ScreenParams;

void DrawUiSliders(int x, int y, BoidParams* params)
{
    char buffer[5];

    GuiSlider((Rectangle){x,y,200,30}, "0", "5", &(params->alignmentFactor), 0.0, 5.0);
    sprintf(buffer, "%.2f", params->alignmentFactor);
    GuiLabel((Rectangle){x,y,200,30}, buffer);

    GuiSlider((Rectangle){x,y+50,200,30}, "0", "5", &(params->cohesionFactor), 0.0, 5.0);
    sprintf(buffer, "%.2f", params->cohesionFactor);
    GuiLabel((Rectangle){x,y+50,200,30}, buffer);

    GuiSlider((Rectangle){x,y+100,200,30}, "0", "20", &(params->separationFactor), 0.0, 20.0);
    sprintf(buffer, "%.2f", params->separationFactor);
    GuiLabel((Rectangle){x,y+100,200,30}, buffer);
}

int main(void)
{
    // Initialization with a hacky fullscreen fix for wasm
    //--------------------------------------------------------------------------------------

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    ScreenParams dimensions = {ScreenWidth: 1920, ScreenHeight: 1080};

    InitWindow(dimensions.ScreenWidth, dimensions.ScreenHeight, "Boids");
    SetWindowMinSize(dimensions.ScreenWidth, dimensions.ScreenHeight);
    SetWindowSize(GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor()));

    RenderTexture2D target = LoadRenderTexture(dimensions.ScreenWidth, dimensions.ScreenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, 0xFF0000FF);
    
    Boid flock[BOID_COUNT];
    memset(flock, 0, sizeof(Boid) * BOID_COUNT);
    
    // Set up params
    BoidParams params;

    params.flockArrayLength = BOID_COUNT;
    params.separationRadius = 30;
    params.visibilityRadius = 100;
    params.maxSpeed = 50.0f;
    params.separationFactor = 10.5f;
    params.alignmentFactor = 0.8f;
    params.cohesionFactor = 0.1f;

    bool drawBoidRadii = false;
    bool drawUiSliders = false;

    QuadTree* qtree = initialiseTree((Rectangle){-dimensions.ScreenWidth/2,-dimensions.ScreenHeight/2, dimensions.ScreenWidth, dimensions.ScreenHeight}, NULL);

    for (int i = 0; i < params.flockArrayLength; i++)
    {
        flock[i] = createBoid((Vector2) {x: dimensions.ScreenWidth, y: dimensions.ScreenHeight},
            &params);
            insertBoidIntoTree(qtree, &flock[i]);
    }

    Camera2D camera = {0};

    camera.target = (Vector2) {0.0f,0.0f};
    camera.offset = (Vector2) {dimensions.ScreenWidth/2.0f, dimensions.ScreenHeight/2.0f};
    camera.zoom = 1.0f;

    SetTargetFPS(60);               // Set the game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        float scale = fmin((float)GetScreenWidth()/dimensions.ScreenWidth, (float)GetScreenHeight()/dimensions.ScreenHeight);

        for(int i = 0; i<BOID_COUNT; i++)
        {
            // TODO
            // I don't know yet why they're getting orphaned yet, but I will need to ensure both the node it was in and the boid itself are accounted for
            if (flock[i].node == NULL)
            {
                printf("Boid with address %x is an orphan\n", &flock[i]);
                insertBoidIntoTree(qtree, &(flock[i]));
            }

            int updated = updateBoid(&flock[i], &params);

            if ( updated != 0)
            {
                printf("Failed to update boid with address %x with exit code %d\n", &flock[i] ,updated);
            }

            if (!(CheckCollisionPointRec(flock[i].position, flock[i].node->bounds)))
            {
                removeBoidFromNode(flock[i].node, &flock[i]);

                if (!(CheckCollisionPointRec(flock[i].position, qtree->bounds)))
                {
                    yeetBoidBackIntoVisibleArea(&flock[i], dimensions.ScreenWidth/2, dimensions.ScreenHeight/2);
                }

                insertBoidIntoTree(qtree, &flock[i]);
            }
        }
    

        if(IsKeyPressed(KEY_R)) drawBoidRadii = !drawBoidRadii;
        if(IsKeyPressed(KEY_S)) drawUiSliders = !drawUiSliders;

        BeginTextureMode(target);

            ClearBackground(DARKGRAY);

            BeginMode2D(camera);

                drawBounds(qtree);

                for(int i = 0; i<BOID_COUNT; i++)
                {
                    drawBoid(&flock[i], &params, drawBoidRadii);
                }

            EndMode2D();
        
        EndTextureMode();


        BeginDrawing();

            ClearBackground(BLACK);

            DrawTexturePro(target.texture, (Rectangle){ 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height },
                           (Rectangle){ (GetScreenWidth() - ((float)dimensions.ScreenWidth*scale))*0.5f, (GetScreenHeight() - ((float)dimensions.ScreenHeight*scale))*0.5f,
                           (float)dimensions.ScreenWidth*scale, (float)dimensions.ScreenHeight*scale }, (Vector2){ 0, 0 }, 0.0f, WHITE);

            if (drawUiSliders) DrawUiSliders(20, dimensions.ScreenHeight/2, &params);

            DrawFPS(10, 10);

        EndDrawing();
        

        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    CloseWindow();              // Close window and OpenGL context

    //--------------------------------------------------------------------------------------

    return 0;
}