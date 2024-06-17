#include "partviewer.h"
#include "catalog.h"

#include "raygui.h"
#include <stddef.h>

#define RECT(x, y, w, h) ((Rectangle){x, y, w, h})

Catalog cat;

const char *options = NULL;
int pickidx = 0;
bool picking = false;

void InitParticleViewer() {
    ParseCatalog(&cat);
    options = TextJoin(cat.names, cat.size, ";");
}

void DrawParticleViewer() {
    GuiLabel(RECT(0, 0, 800, 15), "MP1 - Particle System Viewer");
    if (GuiDropdownBox(RECT(15, 30, 250, 15), options, &pickidx, picking)) {
        picking = !picking;
    }

    GuiStatusBar(RECT(0, 585, 800, 15), TextFormat("current path: %s", cat.paths[pickidx]));
}

void DropParticleViewer() {
    DeleteCatalog(&cat);
}
