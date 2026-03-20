#include <stdlib.h>
#include <raylib.h>
#include <raymath.h>
#include "boid.h"
#include "boidParams.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

typedef struct 
{
    int GameWidth;
    int GameHeight;
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
    ScreenParams dimensions = {GameWidth: 1920, GameHeight: 1080};

    InitWindow(dimensions.GameWidth, dimensions.GameHeight, "Boids");
    SetWindowMinSize(dimensions.GameWidth, dimensions.GameHeight);
    SetWindowSize(GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor()));

    RenderTexture2D target = LoadRenderTexture(dimensions.GameWidth, dimensions.GameHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, 0xFF0000FF);
    
    Boid flock[128];
    
    // Set up params
    BoidParams params;

    params.flockArrayLength = 128;
    params.separationRadius = 30;
    params.visibilityRadius = 100;
    params.maxSpeed = 50.0f;
    params.separationFactor = 10.5f;
    params.alignmentFactor = 0.8f;
    params.cohesionFactor = 0.1f;

    bool drawBoidRadii = false;
    bool drawUiSliders = false;

    for (int i = 0; i < params.flockArrayLength; i++)
    {
        flock[i] = createBoid((Vector2) {x: dimensions.GameHeight, y: dimensions.GameWidth},
            &params,
            flock);
    }

    Camera2D camera = {0};

    camera.target = (Vector2) {0.0f,0.0f};
    camera.offset = (Vector2) {dimensions.GameWidth/2.0f, dimensions.GameHeight/2.0f};
    camera.zoom = 1.0f;

    SetTargetFPS(60);               // Set the game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        float scale = fmin((float)GetScreenWidth()/dimensions.GameWidth, (float)GetScreenHeight()/dimensions.GameHeight);

        // Update boids 
        for(int i = 0; i<128; i++)
        {
            updateBoid(&flock[i], &params);
            yeetBoidBackIntoVisibleArea(&flock[i], dimensions.GameWidth/2, dimensions.GameHeight/2);
        }

        if(IsKeyPressed(KEY_R)) drawBoidRadii = !drawBoidRadii;
        if(IsKeyPressed(KEY_S)) drawUiSliders = !drawUiSliders;

        BeginTextureMode(target);

            ClearBackground(DARKGRAY);

            BeginMode2D(camera);

                for(int i = 0; i<128; i++)
                {
                    drawBoid(&flock[i], &params, drawBoidRadii);
                }

            EndMode2D();
        
        EndTextureMode();


        BeginDrawing();

            ClearBackground(BLACK);

            DrawTexturePro(target.texture, (Rectangle){ 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height },
                           (Rectangle){ (GetScreenWidth() - ((float)dimensions.GameWidth*scale))*0.5f, (GetScreenHeight() - ((float)dimensions.GameHeight*scale))*0.5f,
                           (float)dimensions.GameWidth*scale, (float)dimensions.GameHeight*scale }, (Vector2){ 0, 0 }, 0.0f, WHITE);

            if (drawUiSliders) DrawUiSliders(20, dimensions.GameHeight/2, &params);

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