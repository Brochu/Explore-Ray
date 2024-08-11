#include "iconswatch.h"
#include "partviewer.h"
#include "boids.h"

#include <stdio.h>
#include <time.h>
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define WIDTH 800
#define HEIGHT 600

typedef enum {
    INVALID,
    ICONS,
    MP,
    BOIDS,
} App;

int main(int argc, char **argv) {
    App appId = INVALID;
    time_t t;
    srand((unsigned) time(&t));

    if (argc > 1 && strcmp(argv[1], "-strtest") == 0) {
        char *og = "0123456789012345678901234567890123456789";
        char dest[64];

        size_t s = strlen(og);
        printf("strlen = %zu\n", s);

        strncpy_s(dest, 64, og, s);
        printf("og = '%s', dest = '%s'\n", og, dest);
        return 0;
    }
    else if (argc > 1 && strcmp(argv[1], "-randtest") == 0) {
        float lo = 10.2f;
        float hi = 15.8f;
        for (int i = 0; i < 25; ++i) {
            float r = lo + ((float)rand() / RAND_MAX) * (hi - lo);
            printf("Generated a random number = %f\n", r);
        }
        return 0;
    }

    printf("[MAIN] Program START\n");
    printf("[MAIN] Drawing main window...\n");

    InitWindow(WIDTH, HEIGHT, "[ALPHA] Raylib Exploration");
    SetTargetFPS(60);
    GuiLoadStyle("styles/style_cyber.rgs");

    if (argc > 1 && strcmp(argv[1], "-icons") == 0) {
        appId = ICONS;
    }
    else if (argc > 1 && strcmp(argv[1], "-mp") == 0) {
        appId = MP;
        InitParticleViewer();
    }
    else if (argc > 1 && strcmp(argv[1], "-boids") == 0) {
        appId = BOIDS;
        InitBoidsApp();
    }

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        //TODO: Add handling inputs for each applications
        if (appId == ICONS) {
            DrawIconSwatch();
        }
        else if (appId == MP) {
            DrawParticleViewer();
        }
        else if (appId == BOIDS) {
            DrawBoidsApp();
        }

        EndDrawing();
    }

    if (appId == ICONS) {
    }
    else if (appId == MP) {
        DropParticleViewer();
    }
    else if (appId == BOIDS) {
        DropBoidsApp();
    }

    CloseWindow();
    printf("[MAIN] Program END\n");
    return 0;
}
