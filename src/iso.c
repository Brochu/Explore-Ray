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

typedef size_t tile_t;
tile_t map[MAP_DIMS * MAP_DIMS] = { 0 };

void InitIsoApp() {
    SetTraceLogLevel(LOG_DEBUG);
    TraceLog(LOG_DEBUG, "[ISO] Starting isometric viewer application");
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
    //TODO: Draw map here
    // Highlight correct tile
}

void DropIsoApp() {
    TraceLog(LOG_DEBUG, "[ISO] Dropping isometric viewer application");
}
