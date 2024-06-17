#include "iconswatch.h"
#include "partviewer.h"

#include <stdio.h>
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define WIDTH 800
#define HEIGHT 600

int main(int argc, char **argv) {
    printf("[MAIN] Program START\n");
    printf("[MAIN] Drawing main window...\n");

    InitWindow(WIDTH, HEIGHT, "[ALPHA] Raylib Exploration");
    InitParticleViewer();

    SetTargetFPS(60);
    GuiLoadStyle("styles/style_cyber.rgs");

    while(!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(BLACK);
        if (argc > 1 && strcmp(argv[1], "-icons") == 0) {
            DrawIconSwatch();
        }
        else if (argc > 1 && strcmp(argv[1], "-particles") == 0) {
            DrawParticleViewer();
        }

        EndDrawing();
    }

    DropParticleViewer();
    CloseWindow();
    printf("[MAIN] Program END\n");
    return 0;
}
