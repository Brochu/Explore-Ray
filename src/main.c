#include "iconswatch.h"
#include "partviewer.h"

#include <stdio.h>
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "yaml.h"

#define WIDTH 800
#define HEIGHT 600

int main(int argc, char **argv) {
    printf("[MAIN] Program START\n");
    if (argc > 1 && strcmp(argv[1], "-yaml") == 0) {
        const char *path = "gundata/BombExplo_1EF973EA.gpsm.yaml";
        printf("Loading yaml at '%s'\n", path);

        FILE *file;
        fopen_s(&file, path, "rb");
        char buffer[64];
        fgets(buffer, 64, file);
        printf("READ FROM FILE : '%s'\n", buffer);

        fclose(file);
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
