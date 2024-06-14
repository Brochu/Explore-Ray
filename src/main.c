#include <stdio.h>

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define WIDTH 800
#define HEIGHT 800

#define B_TEXT "#%i# Icon %i"
#define B_WIDTH 75.f
#define B_HEIGHT 30.f

int main(int argc, char **argv) {
    printf("[MAIN] Program START\n");

    InitWindow(WIDTH, HEIGHT, "[ALPHA] Raylib Exploration");
    SetTargetFPS(60);

    while(!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(BLACK);
        DrawText("Hello to first Raylib window", 0, 750, 20, LIGHTGRAY);

        char buttonText[255];
        for (int i = 1; i <= 222; i++) {
            int col = i / 25;
            int row = i % 25;
            sprintf_s(buttonText, 255, B_TEXT, i, i);

            if (GuiButton((Rectangle){(col * B_WIDTH), (row * B_HEIGHT), B_WIDTH, B_HEIGHT}, buttonText)) {
                printf("CLICKED on index = %i!\n", i);
            }
        }

        EndDrawing();
    }

    CloseWindow();

    printf("[MAIN] Program END\n");
    return 0;
}
