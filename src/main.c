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
    if (argc > 1 && strcmp(argv[1], "-strtest") == 0) {
        char *og = "0123456789012345678901234567890123456789";
        char dest[64];

        size_t s = strlen(og);
        printf("strlen = %zu\n", s);

        strncpy_s(dest, 64, og, s);
        printf("og = '%s', dest = '%s'\n", og, dest);
        return 0;
    }

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
