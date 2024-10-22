#include "iso.h"

#include "raylib.h"
#include "raygui.h"

#define RECT(x, y, w, h) ((Rectangle){x, y, w, h})
#define MAP_DIMS 25

typedef struct {
    int x;
    int y;
} pos_t;
pos_t hover = { 0, 0 };

//TODO: Should think of a system to embed this data with the image somehow?
typedef struct {
    int xoffset;
    int yoffset;
    int width;
    int height;
    int size;
    int stride;
} sheet_t;
sheet_t floors = { 0, 7, 160, 80, 29, 5 }; // floors.png
sheet_t exits = { 0, 493, 160, 80, 8, 5 }; // floors.png
sheet_t walls = { 0, 13, 108, 226, 44, 17 }; // walls.png
sheet_t char_n = { 0, 1642, 98, 90, 128, 8 }; // mainchar.gif

typedef size_t tile_t;
tile_t map[MAP_DIMS * MAP_DIMS] = { 0 };

size_t findex = 0;
Texture2D chartex;

void InitIsoApp() {
    SetTraceLogLevel(LOG_DEBUG);
    TraceLog(LOG_DEBUG, "[ISO] Starting isometric viewer application");
    chartex = LoadTexture("isodata\\mainchar.gif");
}

void TickIsoApp() {
    //TODO: Update logic here
    // Find current highlighted coord
}
void DrawIsoApp() {
    GuiStatusBar(
        RECT(0, 585, 800, 15),
        TextFormat("highlight pos (%i, %i); %zu FPS", hover.x, hover.y, GetFPS())
    );

    Rectangle rect = {
        .x = (float)char_n.xoffset,
        .y = (float)char_n.yoffset + (char_n.height * (findex % (char_n.size / char_n.stride))),
        .width = (float)char_n.width,
        .height = (float)char_n.height
    };
    DrawTextureRec(chartex, rect, (Vector2){ .x = 0, .y = 0 }, WHITE);
    findex++;

    //TODO: Draw map here
    // Highlight correct tile
}

void DropIsoApp() {
    TraceLog(LOG_DEBUG, "[ISO] Dropping isometric viewer application");
    UnloadTexture(chartex);
}
