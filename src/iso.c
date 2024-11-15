#include "iso.h"

#include "stdlib.h"
#include "time.h"

#include "raylib.h"
#include "raygui.h"
#include "raymath.h"

#define RECT(x, y, w, h) ((Rectangle){x, y, w, h})
#define VECPOS(v) (int)v.x, (int)v.y
#define MAP_DIMS 20
#define SPRITE_DELAY 6
#define WALK_SPEED 150.f
#define RUN_SPEED 250.f

enum ANIMS {
    AN_IDLE,
    AN_WALK,
    AN_RUN,
    AN_COUNT,
};
Texture2D chartex[AN_COUNT];
size_t findex = 0;
//TODO: look into storing tile coords pos + sub-tile pos separately?
Vector2 charpos;
size_t animidx = 0;

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
sheet_t necro[AN_COUNT] = {
    { 0, 0, 98, 90, 128, 8 }, // necro-neutral.png
    { 0, 0, 93, 97, 128, 8 }, // necro-walk.png
    { 0, 0, 103, 98, 128, 8 }, // necro-run.png
};

typedef struct {
    int x;
    int y;
} pos_t;
pos_t hover = { 0, 0 };

typedef size_t tile_t;
tile_t map[MAP_DIMS * MAP_DIMS];
/*
tile_t map[MAP_DIMS * MAP_DIMS] = {
    00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
    00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
    00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
    00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
    00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
    00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
    00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
    00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
    00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
    00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
};
*/

Vector2 get_tile_pos(sheet_t *tinfo, int x, int y) {
    return (Vector2) {
        .x = (x - y) * tinfo->width/2.f,
        .y = (x + y) * tinfo->height/2.f,
    };
}
pos_t get_tile_coord(sheet_t *tinfo, Vector2 pos) {
    //TODO : v2, works better, but still not ideal
    Vector2Add(pos, (Vector2) { .x = tinfo->width/1.f, .y = tinfo->height/1.f });
    pos.x = pos.x / (tinfo->width);
    pos.y = pos.y / (tinfo->height);

    return (pos_t) {
        .x = (int)(pos.x + pos.y),
        .y = (int)(pos.y - pos.x),
    };
}
bool is_in_tile(sheet_t *tinfo, Vector2 tilepos, Vector2 mouse) {
    float dx = fabsf(mouse.x - tilepos.x);
    float dy = fabsf(mouse.y - tilepos.y);

    return (dx / (tinfo->width/2.f) + dy / (tinfo->height/2.f)) <= 1.f;
}
Vector2 to_tile_space(sheet_t *tinfo, Vector2 pos) {
    //TODO : Opposite of what I thought
    // Need to convert this to matrix mult
    // Need to invert matrix in order to go from screen space to tile space
    float tw = tinfo->width * 0.50f;
    float th = tinfo->height * 0.25f;
    float x = (pos.x * tw) + (pos.y * -tw);
    float y = (pos.x * th) + (pos.y * th);

    return (Vector2) { .x = x, .y = y };
}

Camera2D camera;
Texture2D floortex;
Vector2 mpos;

int find_sprite_index(Vector2 cpos, Vector2 mpos) {
    Vector2 ray = Vector2Subtract(cpos, mpos);

    float angle = -1 * (atan2f(ray.x, ray.y) * (8.f/PI));
    return ((int)(roundf(angle) + 8) % 16);
}

Vector2 sprite_pos(sheet_t *sinfo, Vector2 pos) {
    return Vector2Subtract(pos, (Vector2) { .x = sinfo->width/2.f, sinfo->height/2.f });
}

int dmode = 0;

void InitIsoApp() {
    srand((unsigned int)time(NULL));

    SetTraceLogLevel(LOG_DEBUG);
    TraceLog(LOG_DEBUG, "[ISO] Starting isometric viewer application");

    chartex[AN_IDLE] = LoadTexture("isodata\\necro-idle.png");
    chartex[AN_WALK] = LoadTexture("isodata\\necro-walk.png");
    chartex[AN_RUN] = LoadTexture("isodata\\necro-run.png");
    floortex = LoadTexture("isodata\\floors.png");

    camera = (Camera2D) {
        .offset = (Vector2) { .x = (float)GetScreenWidth()/2, .y = (float)GetScreenHeight()/2 },
        .target = (Vector2) { .x = (float)GetScreenWidth()/2, .y = (float)GetScreenHeight()/2 },
        .rotation = 0.f,
        .zoom = 1.0f,
    };

    charpos = Vector2Zero();
    for (int i = 0; i < MAP_DIMS*MAP_DIMS; ++i) {
        map[i] = rand() % 6;
    }
    map[45] = 11;
    map[46] = 28;
    map[47] = 27;
    map[66] = 26;
    map[67] = 25;
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

    if (Vector2LengthSqr(movement) > EPSILON) {
        movement = Vector2Scale(Vector2Normalize(movement), RUN_SPEED * GetFrameTime());
        if (animidx == 0) {
            animidx = AN_RUN;
            findex = 0;
        }
    } else {
        if (animidx != 0) {
            animidx = AN_IDLE;
            findex = 0;
        }
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        //TODO: Store desired tile/sub-tile positions from mouse pos
        TraceLog(LOG_DEBUG, "[ISO] Mouse clicked, move to position: mpos = (%f, %f)", mpos.x, mpos.y);
    }
    charpos = Vector2Add(charpos, movement);
    camera.target = charpos;
    mpos = GetScreenToWorld2D(GetMousePosition(), camera);

    // Find current highlighted coord
    //TODO: Need to convert this to a function to avoid GOTO lul
    for (int y = 0; y < MAP_DIMS; ++y) {
        for (int x = 0; x < MAP_DIMS; ++x) {
            Vector2 tilepos = get_tile_pos(&floors, x, y);
            if (is_in_tile(&floors, tilepos, mpos)) {
                hover = (pos_t) { .x = x, .y = y };
                goto end;
            }
        }
    }
    hover = (pos_t) { .x = -1, .y = -1 };
end:
    (hover);
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
            bool highlight = false;
            if (hover.x == x && hover.y == y) { highlight = true; }
            DrawTextureRec(floortex, rect, sprite_pos(&floors, pos), (highlight) ? GRAY : WHITE);
        }
    }

    if (dmode == 1) {
        for (int y = MAP_DIMS-1; y >= 0; --y) {
            for (int x = MAP_DIMS-1; x >= 0; --x) {
                Vector2 debugpos = get_tile_pos(&floors, x, y);
                size_t tileidx = map[(y * MAP_DIMS) + x];
                size_t xoff = tileidx % floors.stride;
                size_t yoff = tileidx / floors.stride;
                const char *output = TextFormat("[%zu](%zu, %zu)", tileidx, xoff, yoff);
                DrawText(output, VECPOS(debugpos), 8, WHITE);

                Vector2 l = Vector2Subtract(debugpos, (Vector2) { .x = floors.width/2.f, .y = 0.f });
                Vector2 t = Vector2Subtract(debugpos, (Vector2) { .x = 0.f, .y = floors.height/2.f });
                DrawLineV(l, t, GRAY);
                Vector2 r = Vector2Add(debugpos, (Vector2) { .x = floors.width/2.f, .y = 0.f });
                DrawLineV(t, r, GRAY);
                Vector2 b = Vector2Add(debugpos, (Vector2) { .x = 0.f, .y = floors.height/2.f });
                DrawLineV(r, b, GRAY);
                DrawLineV(b, l, GRAY);
            }
        }
    }

    // SHADOW
    Vector2 shadowpos = Vector2Add(charpos, (Vector2){ .x = 0.f, .y = 40.f });
    DrawEllipse(VECPOS(shadowpos), 20.f, 8.f, ColorAlpha(BLACK, 0.5f));

    // CHAR
    int index = find_sprite_index(charpos, mpos);
    sheet_t *sheet = &necro[animidx];
    rect = (Rectangle) {
        .x = (float)sheet->xoffset + (sheet->width * ((findex / SPRITE_DELAY) % sheet->stride)),
        .y = (float)sheet->yoffset + (sheet->height * index),
        .width = (float)sheet->width,
        .height = (float)sheet->height
    };
    DrawTextureRec(chartex[animidx], rect, sprite_pos(sheet, charpos), WHITE);

    if (dmode == 1) {
        DrawCircle(VECPOS(mpos), 5.f, RED);
        DrawLine(VECPOS(charpos), VECPOS(mpos), BLUE);
    }
    EndMode2D();

    Vector2 tilespace = to_tile_space(&floors, (Vector2){ 1.f, 0.f });
    GuiStatusBar(RECT(0, 585, 800, 15),
        TextFormat("mouse pos (%.03f, %.03f); tilespace (%.03f, %.03f); highlight pos (%i, %i); %i FPS; Sprite Index = %i",
            mpos.x, mpos.y,
            tilespace.x, tilespace.y,
            hover.x, hover.y,
            GetFPS(),
            index)
    );
    findex++;
}

void DropIsoApp() {
    TraceLog(LOG_DEBUG, "[ISO] Dropping isometric viewer application");
    UnloadTexture(floortex);
    UnloadTexture(chartex[AN_RUN]);
    UnloadTexture(chartex[AN_WALK]);
    UnloadTexture(chartex[AN_IDLE]);
}
