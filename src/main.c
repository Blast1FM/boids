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
		flock[i] = *createBoid((Vector2){GetRandomValue(-screenWidth, screenWidth), GetRandomValue(-screenHeight, screenHeight)}, (Vector2){GetRandomValue(-0.5,0.5), GetRandomValue(-0.5,0.5)}, flock);

    Rectangle player = {0,0,50,50};

    float playerSpeed = 150.0f;

    Vector2 playerVelocity = {0.0f,0.0f};

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

        if(IsKeyDown(KEY_A)) playerVelocity.x = -playerSpeed;
        if(IsKeyDown(KEY_D)) playerVelocity.x = playerSpeed;

        if(IsKeyDown(KEY_W)) playerVelocity.y = -playerSpeed;
        if(IsKeyDown(KEY_S)) playerVelocity.y = playerSpeed;

        if(!(IsKeyDown(KEY_A) || IsKeyDown(KEY_D))) playerVelocity.x = 0;
        if(!(IsKeyDown(KEY_W) || IsKeyDown(KEY_S))) playerVelocity.y = 0;

        playerVelocity = Vector2Scale(Vector2Normalize(playerVelocity), playerSpeed);

        player.x = player.x + playerVelocity.x * GetFrameTime();
        player.y = player.y + playerVelocity.y * GetFrameTime();

        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode2D(camera);

                for(int i = 0; i<128; i++)
                {
                    drawBoid(&flock[i]);
                }

                DrawRectangleRec(player, RED);

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