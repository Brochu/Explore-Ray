#include <stdio.h>

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define WIDTH 800
#define HEIGHT 800

#define B_TEXT "#%i# Icon %i"
#define B_WIDTH 75.f
#define B_HEIGHT 30.f

bool checked = false;

int main(int argc, char **argv) {
    printf("[MAIN] Program START\n");

    InitWindow(WIDTH, HEIGHT, "[ALPHA] Raylib Exploration");
    SetTargetFPS(60);

    while(!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(BLACK);
        DrawText("Hello to first Raylib window", 0, 750, 20, LIGHTGRAY);

        char buttonText[255];
        for (int i = 0; i < 222; i++) {
            int col = i / 25;
            int row = i % 25;
            sprintf_s(buttonText, 255, B_TEXT, i, i);

            if (GuiButton((Rectangle){(col * B_WIDTH), (row * B_HEIGHT), B_WIDTH, B_HEIGHT}, buttonText)) {
                printf("CLICKED on index = %i!\n", i);
            }
        }

        if (GuiLabelButton( (Rectangle){150, 775, 130, 30}, "This label clickable" )) {
            printf("Label clicked\n");
        }
        if (GuiCheckBox( (Rectangle){300, 775, 20, 20}, "CHECKBOX", &checked)) {
            printf("Checkbox %s\n", checked ? "CHECKED" : "UNCHECKED");
        }

        EndDrawing();
    }

    CloseWindow();

    printf("[MAIN] Program END\n");
    return 0;
}
