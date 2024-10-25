#include "iso.h"

#include "math.h"
#include "raylib.h"
#include "raygui.h"
#include "raymath.h"

#define RECT(x, y, w, h) ((Rectangle){x, y, w, h})
#define VECPOS(v) (int)v.x, (int)v.y
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
sheet_t char_r = { 0, 0, 103, 98, 128, 8 }; // run.png

typedef size_t tile_t;
tile_t map[MAP_DIMS * MAP_DIMS] = { 0 };

size_t findex = 0;
Texture2D chartex;

void InitIsoApp() {
    SetTraceLogLevel(LOG_DEBUG);
    TraceLog(LOG_DEBUG, "[ISO] Starting isometric viewer application");
    chartex = LoadTexture("isodata\\run.png");
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

    Vector2 center = { .x = (float)GetScreenWidth()/2, .y = (float)GetScreenHeight()/2 };
    Vector2 pos = Vector2Subtract(center, (Vector2) { .x = (float)char_r.width/2, .y = (float)char_r.height/2 });
    Vector2 mpos = GetMousePosition();
    Vector2 txtpos = Vector2Add(mpos, (Vector2){ .x = 0, .y = -20 });
    DrawCircle(VECPOS(mpos), 5.f, RED);
    DrawLine(VECPOS(center), VECPOS(mpos), BLUE);
    Vector2 ray = Vector2Subtract(center, mpos);
    float angle = atan2f(ray.x, ray.y) * (8.f/PI);
    float index = roundf(angle) + 8;
    DrawText(TextFormat("%f : %f", angle, index), VECPOS(txtpos), 15, WHITE);

    Rectangle rect = {
        .x = (float)char_r.xoffset + (char_r.width * ((findex / 6) % char_r.stride)),
        .y = (float)char_r.yoffset + (char_r.height * (16-index)),
        .width = (float)char_r.width,
        .height = (float)char_r.height
    };
    DrawTextureRec(chartex, rect, pos, WHITE);
    findex++;

    //TODO: Draw map here
    // Highlight correct tile
}

void DropIsoApp() {
    TraceLog(LOG_DEBUG, "[ISO] Dropping isometric viewer application");
    UnloadTexture(chartex);
}
