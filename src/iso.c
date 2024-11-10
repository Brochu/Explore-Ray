#include "iso.h"

#include "raylib.h"
#include "raygui.h"
#include "raymath.h"

#define RECT(x, y, w, h) ((Rectangle){x, y, w, h})
#define VECPOS(v) (int)v.x, (int)v.y
#define MAP_DIMS 25
#define SPRITE_DELAY 6
#define SPEED 5

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
sheet_t necro_n = { 0, 1642, 98, 90, 128, 8 }; // necro.gif
sheet_t necro_w = { 0, 0, 93, 97, 128, 8 }; // necro-walk.gif

typedef size_t tile_t;
tile_t map[MAP_DIMS * MAP_DIMS] = { 0 };

size_t findex = 0;
Texture2D chartex;
Texture2D floortex;

int find_sprite_index(Vector2 cpos, Vector2 mpos) {
    Vector2 ray = Vector2Subtract(cpos, mpos);

    float angle = -1 * (atan2f(ray.x, ray.y) * (8.f/PI));
    return ((int)(roundf(angle) + 8) % 16);
}

Camera2D camera;
Vector2 center;
Vector2 charpos;
Vector2 mpos;

int dmode = 0;

void InitIsoApp() {
    SetTraceLogLevel(LOG_DEBUG);
    TraceLog(LOG_DEBUG, "[ISO] Starting isometric viewer application");
    chartex = LoadTexture("isodata\\necro-walk.png");
    floortex = LoadTexture("isodata\\floors.png");

    camera = (Camera2D) {
        .offset = (Vector2) { .x = (float)GetScreenWidth()/2, .y = (float)GetScreenHeight()/2 },
        .target = (Vector2) { .x = (float)GetScreenWidth()/2, .y = (float)GetScreenHeight()/2 },
        .rotation = 0.f,
        .zoom = 1.0f,
    };
    center = (Vector2) { .x = (float)GetScreenWidth()/2, .y = (float)GetScreenHeight()/2 };
    //TODO: Try to move this to center now that we have camera?
    // Need to understand how to get mouse position after camera transform
    /*
    RLAPI Vector2 GetScreenToWorld2D(Vector2 position, Camera2D camera);    // Get the world space position for a 2d camera screen space position
    RLAPI Matrix GetCameraMatrix2D(Camera2D camera);                        // Get camera 2d transform matrix
    // */
    charpos = center;
}

void TickIsoApp() {
    //TODO: Update logic here
    // Find current highlighted coord

    if (IsKeyPressed(KEY_R)) {
        //TODO: Handle mode draw modes later?
        if (dmode == 0) dmode = 1;
        else if (dmode == 1) dmode = 0;
    }

    Vector2 movement = Vector2Zero();
    float unit = 1.f;
    if (IsKeyDown(KEY_W)) {
        movement.y -= unit;
    }
    if (IsKeyDown(KEY_S)) {
        movement.y += unit;
    }
    if (IsKeyDown(KEY_A)) {
        movement.x -= unit;
    }
    if (IsKeyDown(KEY_D)) {
        movement.x += unit;
    }
    movement = Vector2Scale(Vector2Normalize(movement), SPEED);

    //TODO: Add deltatime here
    charpos = Vector2Add(charpos, movement);
    camera.target = charpos;
    mpos = GetMousePosition();
}

void DrawIsoApp() {
    Rectangle rect;
    Vector2 pos;

    BeginMode2D(camera);
    //TODO: Draw map here
    // Highlight correct tile
    rect = (Rectangle) {
        .x = (float)floors.xoffset,
        .y = (float)floors.yoffset + (floors.height * ((findex / SPRITE_DELAY) % 5)),
        .width = (float)floors.width,
        .height = (float)floors.height
    };
    pos = Vector2Add(center, (Vector2) { .x = 0, .y = 150 });
    DrawTextureRec(floortex, rect, pos, WHITE);

    int index = find_sprite_index(charpos, mpos);
    rect = (Rectangle) {
        .x = (float)necro_w.xoffset + (necro_w.width * ((findex / SPRITE_DELAY) % necro_w.stride)),
        .y = (float)necro_w.yoffset + (necro_w.height * index),
        .width = (float)necro_w.width,
        .height = (float)necro_w.height
    };
    DrawTextureRec(chartex, rect, Vector2Subtract(charpos, (Vector2) {.x = necro_w.width/2.f, .y = necro_w.height/2.f}), WHITE);

    if (dmode == 1) {
        DrawCircle(VECPOS(mpos), 5.f, RED);
        DrawLine(VECPOS(charpos), VECPOS(mpos), BLUE);
    }
    EndMode2D();

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
    UnloadTexture(floortex);
    UnloadTexture(chartex);
}
