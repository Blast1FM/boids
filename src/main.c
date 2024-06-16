#include <stdlib.h>
#include "raylib.h"
#include "raymath.h"
#include "boid.h"

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;

    SetConfigFlags(FLAG_MSAA_4X_HINT);

    InitWindow(screenWidth, screenHeight, "Boids");

    Boid flock[128];

    for (int i = 0; i < 128; i++)
		flock[i] = *createBoid(
            (Vector2){GetRandomValue(-screenWidth, screenWidth),
            GetRandomValue(-screenHeight, screenHeight)}, 
            (Vector2){GetRandomValue(-10,10), 
            GetRandomValue(-10,10)}, 
            flock);

    Camera2D camera = {0};

    camera.target = (Vector2) {0.0f,0.0f};
    camera.offset = (Vector2) {screenWidth/2.0f, screenHeight/2.0f};
    camera.zoom = 1.0f;

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update

        for(int i = 0; i<128; i++)
        {
            updateBoid(&flock[i], 128);
        }

        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode2D(camera);

                for(int i = 0; i<128; i++)
                {
                    drawBoid(&flock[i]);
                }

            EndMode2D();

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