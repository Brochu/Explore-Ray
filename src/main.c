#include <stdio.h>

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define WIDTH 800
#define HEIGHT 800

int main(int argc, char **argv) {
    printf("[MAIN] Program START\n");

    InitWindow(WIDTH, HEIGHT, "[ALPHA] Raylib Exploration");
    SetTargetFPS(60);

    while(!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(BLACK);
        DrawText("Hello to first Raylib window", 0, 0, 20, LIGHTGRAY);

        if (GuiButton((Rectangle){24, 24, 130, 30}, "#191#Output to cmdline")) {
            printf("CLICKED!\n");
        }

        EndDrawing();
    }

    CloseWindow();

    printf("[MAIN] Program END\n");
    return 0;
}
