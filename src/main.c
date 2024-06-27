#include <stdlib.h>
#include "raylib.h"
#include "raymath.h"
#include "boid.h"
#include "boidParams.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

int main(void)
{
    // Initialization with a hacky fullscreen fix for wasm
    //--------------------------------------------------------------------------------------

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(100, 100, "Boids");

    const int screenWidth = GetMonitorWidth(GetCurrentMonitor());
    const int screenHeight = GetMonitorHeight(GetCurrentMonitor());

    SetWindowSize(screenWidth, screenHeight);

    Boid flock[128];
    
    BoidParams* params = malloc(sizeof(BoidParams));

    params->flockArrayLength = 128;
    params->separationRadius = 30;
    params->visibilityRadius = 100;
    params->maxSpeed = 50.0f;
    params->separationFactor = 10.5f;
    params->alignmentFactor = 0.8f;
    params->cohesionFactor = 0.1f;

    bool drawBoidRadii = false;

    for (int i = 0; i < params->flockArrayLength; i++)
		flock[i] = *createBoid(
            (Vector2){GetRandomValue(-screenWidth/2, screenWidth/2),
            GetRandomValue(-screenHeight/2, screenHeight/2)}, 
            (Vector2){GetRandomValue(-10,10), 
            GetRandomValue(-10,10)}, 
            flock);

    Camera2D camera = {0};

    camera.target = (Vector2) {0.0f,0.0f};
    camera.offset = (Vector2) {screenWidth/2.0f, screenHeight/2.0f};
    camera.zoom = 1.0f;

    SetTargetFPS(60);               // Set the game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    printf("Width: %d, Height: %d\n", screenWidth, screenHeight);
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        for(int i = 0; i<128; i++)
        {
            updateBoid(&flock[i], params);
            yeetBoidBackIntoVisibleArea(&flock[i], screenWidth, screenHeight);
        }

        if(IsKeyPressed(KEY_R)) drawBoidRadii = !drawBoidRadii;

        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode2D(camera);

                for(int i = 0; i<128; i++)
                {
                    drawBoid(&flock[i], params, drawBoidRadii);
                }

            EndMode2D();

            DrawFPS(10, 10);
        
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    CloseWindow();              // Close window and OpenGL context
    free(params);               // Deallocate params

    //--------------------------------------------------------------------------------------

    return 0;
}