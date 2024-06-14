#include <stdio.h>

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define WIDTH 800
#define HEIGHT 800

#define BUTTON_TEXT "#%i# Icon %i"
#define BUTTON_WIDTH 75

int main(int argc, char **argv) {
    printf("[MAIN] Program START\n");

    InitWindow(WIDTH, HEIGHT, "[ALPHA] Raylib Exploration");
    SetTargetFPS(60);

    while(!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(BLACK);
        DrawText("Hello to first Raylib window", 0, 0, 20, LIGHTGRAY);

        char buttonText[255];
        for (int i = 1; i <= 25; i++) {
            int offset = i;
            sprintf_s(buttonText, 255, BUTTON_TEXT, offset, offset);

            if (GuiButton((Rectangle){25, (i * 30), BUTTON_WIDTH, 30}, buttonText)) {
                printf("CLICKED on index = %i!\n", offset);
            }
        }

        for (int i = 1; i <= 25; i++) {
            int offset = i + 25;
            sprintf_s(buttonText, 255, BUTTON_TEXT, offset, offset);

            if (GuiButton((Rectangle){100, (i * 30), BUTTON_WIDTH, 30}, buttonText)) {
                printf("CLICKED on index = %i!\n", offset);
            }
        }

        for (int i = 1; i <= 25; i++) {
            int offset = i + 50;
            sprintf_s(buttonText, 255, BUTTON_TEXT, offset, offset);

            if (GuiButton((Rectangle){175, (i * 30), BUTTON_WIDTH, 30}, buttonText)) {
                printf("CLICKED on index = %i!\n", offset);
            }
        }

        for (int i = 1; i <= 25; i++) {
            int offset = i + 75;
            sprintf_s(buttonText, 255, BUTTON_TEXT, offset, offset);

            if (GuiButton((Rectangle){250, (i * 30), BUTTON_WIDTH, 30}, buttonText)) {
                printf("CLICKED on index = %i!\n", offset);
            }
        }

        for (int i = 1; i <= 25; i++) {
            int offset = i + 100;
            sprintf_s(buttonText, 255, BUTTON_TEXT, offset, offset);

            if (GuiButton((Rectangle){325, (i * 30), BUTTON_WIDTH, 30}, buttonText)) {
                printf("CLICKED on index = %i!\n", offset);
            }
        }

        for (int i = 1; i <= 25; i++) {
            int offset = i + 125;
            sprintf_s(buttonText, 255, BUTTON_TEXT, offset, offset);

            if (GuiButton((Rectangle){400, (i * 30), BUTTON_WIDTH, 30}, buttonText)) {
                printf("CLICKED on index = %i!\n", offset);
            }
        }

        for (int i = 1; i <= 25; i++) {
            int offset = i + 150;
            sprintf_s(buttonText, 255, BUTTON_TEXT, offset, offset);

            if (GuiButton((Rectangle){475, (i * 30), BUTTON_WIDTH, 30}, buttonText)) {
                printf("CLICKED on index = %i!\n", offset);
            }
        }

        for (int i = 1; i <= 25; i++) {
            int offset = i + 175;
            sprintf_s(buttonText, 255, BUTTON_TEXT, offset, offset);

            if (GuiButton((Rectangle){550, (i * 30), BUTTON_WIDTH, 30}, buttonText)) {
                printf("CLICKED on index = %i!\n", offset);
            }
        }

        for (int i = 1; i <= 25; i++) {
            int offset = i + 200;
            sprintf_s(buttonText, 255, BUTTON_TEXT, offset, offset);

            if (GuiButton((Rectangle){625, (i * 30), BUTTON_WIDTH, 30}, buttonText)) {
                printf("CLICKED on index = %i!\n", offset);
            }
        }

        EndDrawing();
    }

    CloseWindow();

    printf("[MAIN] Program END\n");
    return 0;
}
