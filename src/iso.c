#include "iso.h"

#include "stdlib.h"
#include "time.h"

#include "raylib.h"
#include "raygui.h"
#include "raymath.h"

#define RECT(x, y, w, h) ((Rectangle){x, y, w, h})
#define VECPOS(v) (int)v.x, (int)v.y
#define MAP_DIMS 10
#define SPRITE_DELAY 6
#define SPEED 250.f

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
sheet_t floors = { 0, 8, 160, 80, 29, 5 }; // floors.png
sheet_t exits = { 0, 493, 160, 80, 8, 5 }; // floors.png
sheet_t walls = { 0, 13, 108, 226, 44, 17 }; // walls.png
sheet_t necro_w = { 0, 0, 93, 97, 128, 8 }; // necro-walk.gif
sheet_t necro_r = { 0, 0, 103, 98, 128, 8 }; // necro-walk.gif

typedef size_t tile_t;
tile_t map[MAP_DIMS * MAP_DIMS] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

Vector2 get_tile_pos(sheet_t *tinfo, int x, int y) {
    return (Vector2) {
        .x = (x - y) * tinfo->width/2.f,
        .y = (x + y) * tinfo->height/2.f,
    };
}
pos_t get_tile_coord(sheet_t *tinfo, Vector2 pos) {
    //TODO : this might need some work, not very accurate
    pos.x = pos.x / (tinfo->width);
    pos.y = pos.y / (tinfo->height);

    return (pos_t) {
        .x = (int)(pos.x + pos.y),
        .y = -1 * (int)(pos.x - pos.y),
    };
}

size_t findex = 0;
Texture2D chartex;
Texture2D floortex;

int find_sprite_index(Vector2 cpos, Vector2 mpos) {
    Vector2 ray = Vector2Subtract(cpos, mpos);

    float angle = -1 * (atan2f(ray.x, ray.y) * (8.f/PI));
    return ((int)(roundf(angle) + 8) % 16);
}

Vector2 sprite_pos(sheet_t *sinfo, Vector2 pos) {
    return Vector2Subtract(pos, (Vector2) { .x = sinfo->width/2.f, sinfo->height/2.f });
}

Camera2D camera;
Vector2 charpos;
Vector2 mpos;

int dmode = 0;

void InitIsoApp() {
    srand((unsigned int)time(NULL));

    SetTraceLogLevel(LOG_DEBUG);
    TraceLog(LOG_DEBUG, "[ISO] Starting isometric viewer application");

    chartex = LoadTexture("isodata\\necro-run.png");
    floortex = LoadTexture("isodata\\floors.png");

    camera = (Camera2D) {
        .offset = (Vector2) { .x = (float)GetScreenWidth()/2, .y = (float)GetScreenHeight()/2 },
        .target = (Vector2) { .x = (float)GetScreenWidth()/2, .y = (float)GetScreenHeight()/2 },
        .rotation = 0.f,
        .zoom = 1.0f,
    };

    charpos = Vector2Zero();
    for (int i = 0; i < MAP_DIMS*MAP_DIMS; ++i) {
        map[i] = rand() % 21;
    }
}

void TickIsoApp() {
    if (IsKeyPressed(KEY_R)) {
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
    movement = Vector2Scale(Vector2Normalize(movement), SPEED * GetFrameTime());

    charpos = Vector2Add(charpos, movement);
    camera.target = charpos;
    mpos = GetScreenToWorld2D(GetMousePosition(), camera);

    // Find current highlighted coord
    hover = get_tile_coord(&floors, mpos);
}

void DrawIsoApp() {
    Rectangle rect;
    Vector2 pos;

    BeginMode2D(camera);
    for (int y = MAP_DIMS-1; y >= 0; --y) {
        for (int x = MAP_DIMS-1; x >= 0; --x) {
            size_t tileidx = map[(y * MAP_DIMS) + x];
            size_t xoff = tileidx % floors.stride;
            size_t yoff = tileidx / floors.stride;
            rect = (Rectangle) {
                .x = (float)floors.xoffset + (xoff * floors.width),
                .y = (float)floors.yoffset + (yoff * floors.height),
                .width = (float)floors.width,
                .height = (float)floors.height
            };
            pos = get_tile_pos(&floors, x, y);
            DrawTextureRec(floortex, rect, pos, WHITE);
        }
    }

    for (int y = MAP_DIMS-1; y >= 0; --y) {
        for (int x = MAP_DIMS-1; x >= 0; --x) {
            if (dmode == 1) {
                Vector2 debugpos = Vector2Subtract(get_tile_pos(&floors, x, y), (Vector2){ .x = -50.f, .y = -25.f });
                size_t tileidx = map[(y * MAP_DIMS) + x];
                size_t xoff = tileidx % floors.stride;
                size_t yoff = tileidx / floors.stride;
                const char *output = TextFormat("[%zu](%zu, %zu)", tileidx, xoff, yoff);
                DrawText(output, VECPOS(debugpos), 8, WHITE);
                DrawCircle(VECPOS(mpos), 5.f, RED);
                DrawLine(VECPOS(charpos), VECPOS(mpos), BLUE);
            }
        }
    }

    // SHADOW
    Vector2 shadowpos = Vector2Add(charpos, (Vector2){ .x = 0.f, .y = 45.f });
    DrawEllipse(VECPOS(shadowpos), 20.f, 8.f, ColorAlpha(BLACK, 0.5f));

    // CHAR
    int index = find_sprite_index(charpos, mpos);
    rect = (Rectangle) {
        .x = (float)necro_r.xoffset + (necro_r.width * ((findex / SPRITE_DELAY) % necro_r.stride)),
        .y = (float)necro_r.yoffset + (necro_r.height * index),
        .width = (float)necro_r.width,
        .height = (float)necro_r.height
    };
    DrawTextureRec(chartex, rect, sprite_pos(&necro_r, charpos), WHITE);

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
