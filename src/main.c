#include <stdio.h>

#include "iconswatch.h"
#include "catalog.h"

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define WIDTH 800
#define HEIGHT 600

int main(int argc, char **argv) {
    printf("[MAIN] Program START\n");
    printf("[MAIN] Parsing particle effects catalog...\n");
    Catalog c = ParseCatalog();
    for (int i = 0; i < c.size; ++i) {
        printf("'%s', ", c.names[i]);
    }
    printf("\n[MAIN] DONE (%zu items parsed)\n", c.size);

    printf("[MAIN] Drawing main window...\n");
    InitWindow(WIDTH, HEIGHT, "[ALPHA] Raylib Exploration");
    SetTargetFPS(60);
    GuiLoadStyle("styles/style_cyber.rgs");

    while(!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(BLACK);
        DrawIconSwatch();

        EndDrawing();
    }

    CloseWindow();
    DeleteCatalog(&c);

    printf("[MAIN] DONE\n");
    printf("[MAIN] Program END\n");
    return 0;
}
