#include <stdlib.h>
#include <raylib.h>
#include "raymath.h"
#include "boid.h"
#include "boidParams.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

typedef struct 
{
    int Width;
    int Height;
    int HalfWidth;
    int HalfHeight;
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

    InitWindow(100, 100, "Boids");

    ScreenParams dimensions = {Width: GetMonitorWidth(GetCurrentMonitor()), Height: GetMonitorHeight(GetCurrentMonitor())};

    dimensions.HalfHeight = dimensions.Height/2;
    dimensions.HalfWidth = dimensions.Width/2;

    GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, 0xFF0000FF);
    
    SetWindowSize(dimensions.Width, dimensions.Height);

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
    bool drawUiSliders = false;

    for (int i = 0; i < params->flockArrayLength; i++)
		flock[i] = *createBoid(
            (Vector2){GetRandomValue(-dimensions.Height/2, dimensions.Width/2),
            GetRandomValue(-dimensions.Height/2, dimensions.Width/2)}, 
            (Vector2){GetRandomValue(-10,10), 
            GetRandomValue(-10,10)}, 
            flock);

    Camera2D camera = {0};

    camera.target = (Vector2) {0.0f,0.0f};
    camera.offset = (Vector2) {dimensions.Width/2.0f, dimensions.Height/2.0f};
    camera.zoom = 1.0f;

    SetTargetFPS(60);               // Set the game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        if (IsWindowResized())
        {
            dimensions.Height = GetScreenHeight();
            dimensions.Width = GetScreenWidth();

            dimensions.HalfHeight = dimensions.Height/2;
            dimensions.HalfWidth = dimensions.Width/2;

            camera.offset = (Vector2) {dimensions.HalfHeight, dimensions.HalfWidth};
        }

        if (drawUiSliders) DrawUiSliders(20, dimensions.HalfHeight, params);

        // Update
        for(int i = 0; i<128; i++)
        {
            updateBoid(&flock[i], params);
            yeetBoidBackIntoVisibleArea(&flock[i], dimensions.HalfWidth, dimensions.HalfHeight);
        }

        if(IsKeyPressed(KEY_R)) drawBoidRadii = !drawBoidRadii;
        if(IsKeyPressed(KEY_S)) drawUiSliders = !drawUiSliders;

        BeginDrawing();

            ClearBackground(DARKGRAY);

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