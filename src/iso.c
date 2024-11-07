#include "iso.h"

#include "math.h"
#include "raylib.h"
#include "raygui.h"
#include "raymath.h"

#define RECT(x, y, w, h) ((Rectangle){x, y, w, h})
#define VECPOS(v) (int)v.x, (int)v.y
#define MAP_DIMS 25
#define SPRITE_DELAY 6

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

int find_sprite_index(Vector2 cpos, Vector2 mpos) {
    Vector2 ray = Vector2Subtract(cpos, mpos);

    float angle = -1 * (atan2f(ray.x, ray.y) * (8.f/PI));
    return ((int)(roundf(angle) + 8) % 16);
}

int dmode = 0;

void InitIsoApp() {
    SetTraceLogLevel(LOG_DEBUG);
    TraceLog(LOG_DEBUG, "[ISO] Starting isometric viewer application");
    chartex = LoadTexture("isodata\\mainchar.gif");
}

void TickIsoApp() {
    //TODO: Update logic here
    // Find current highlighted coord

    if (IsKeyPressed(KEY_R)) {
        //TODO: Handle mode draw modes later?
        if (dmode == 0) dmode = 1;
        else if (dmode == 1) dmode = 0;
    }
}
void DrawIsoApp() {
    Vector2 center = { .x = (float)GetScreenWidth()/2, .y = (float)GetScreenHeight()/2 };
    Vector2 mpos = GetMousePosition();
    int index = find_sprite_index(center, mpos);

    Rectangle rect = {
        .x = (float)char_n.xoffset + (char_n.width * ((findex / 6) % char_n.stride)),
        .y = (float)char_n.yoffset + (char_n.height * index),
        .width = (float)char_n.width,
        .height = (float)char_n.height
    };
    Vector2 pos = Vector2Subtract(center, (Vector2) { .x = (float)char_n.width/2, .y = (float)char_n.height/2 });
    DrawTextureRec(chartex, rect, pos, WHITE);

    if (dmode == 1) {
        DrawCircle(VECPOS(mpos), 5.f, RED);
        DrawLine(VECPOS(center), VECPOS(mpos), BLUE);
    }

    //TODO: Draw map here
    // Highlight correct tile

    GuiStatusBar(RECT(0, 585, 800, 15),
        TextFormat("highlight pos (%i, %i); %zu FPS; Sprite Index = %i",
            hover.x, hover.y,
            GetFPS(),
            index)
    );
    findex++;
}

void DropIsoApp() {
    TraceLog(LOG_DEBUG, "[ISO] Dropping isometric viewer application");
    UnloadTexture(chartex);
}
