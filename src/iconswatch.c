#include "iconswatch.h"

#include <stdio.h>
#include "raygui.h"

#define RECT(x, y, w, h) ((Rectangle){x, y, w, h})
#define B_TEXT "#%i# Icon %i"
#define B_WIDTH 85.f
#define B_HEIGHT 25.f

#define PAD 15.f
#define XMAR 144.f
#define YMAR 15.f

#define ROWS 20
#define PAGE_SIZE 120

int page = 0;

void DrawIconSwatch() {
    GuiLabel(RECT(0, 0, 800, 15), "Raygui icons viewer");

    char buttonText[255];
    for (int i = 0; i < PAGE_SIZE; i++) {
        int icon = i + (PAGE_SIZE * page);
        sprintf_s(buttonText, 255, B_TEXT, icon, icon);

        int col = i / ROWS;
        int row = i % ROWS;
        float x = PAD + XMAR + col * B_WIDTH;
        float y = PAD + YMAR + row * B_HEIGHT;

        if (GuiButton(RECT(x, y, B_WIDTH, B_HEIGHT), buttonText)) {
            printf("CLICKED on icon #%i\n", icon);
        }
    }

    sprintf_s(buttonText, 255, "%i", page);
    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    if (GuiButton(RECT(338, 550, 50, 25), "<") && page == 1) {
        page--;
    }
    GuiLabel(RECT(388, 550, 25, 25), buttonText);
    if (GuiButton(RECT(413, 550, 50, 25), ">") && page == 0) {
        page++;
    }
    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
}
