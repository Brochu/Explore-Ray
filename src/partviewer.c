#include "partviewer.h"
#include "catalog.h"
#include "partfx.h"

#include "raygui.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define FOLDER "gundata"
#define RECT(x, y, w, h) ((Rectangle){x, y, w, h})

Catalog cat;
partfx_t fx;

const char *options = NULL;
int pickidx = 10;
bool picking = false;
char *data = NULL;

Vector2 scroll = { 0 };
Rectangle view = { 0 };

void LoadParticleEffect(int idx) {
    if (data != NULL) {
        UnloadFileText(data);
        partfx_reset(&fx);
    }

    char path[128];
    sprintf_s(path, 128, "%s/%s", FOLDER, cat.paths[pickidx]);
    data = LoadFileText(path);
    partfx_parse(&fx, data, strlen(data));

    printf("[FX] The parsed partfx has %zu properties\n", fx._prop_len);
    for (int i = 0; i < fx._prop_len; ++i) {
        printf("[PROP] name = '%s'\n", fx._props[i]->type);
    }
}

void InitParticleViewer() {
    ParseCatalog(&cat);
    partfx_init(&fx);

    options = TextJoin(cat.names, (int)cat.size, ";");
    LoadParticleEffect(pickidx);
}

void DrawParticleViewer() {
    GuiLabel(RECT(0, 0, 800, 15), "MP1 - Particle System Viewer");
    GuiStatusBar(RECT(0, 585, 800, 15), TextFormat("current file: %s", cat.paths[pickidx]));

    GuiScrollPanel(RECT(15, 60, 775, 450), NULL, RECT(0, 0, 340, 340), &scroll, &view);

    char contents[128];
    int lifetime = 0;
    int maxparticles = 0;
    char *texname = NULL;
    float random = 0.f;
    Vector4 color = { 0 };
    //partfx_query(&fx, LIFETIME, &lifetime);
    //partfx_query(&fx, MAX_PARTICLES, &maxparticles);
    //partfx_query(&fx, TEXTURE, &texname);
    //partfx_query(&fx, GEN_RATE, &random);
    //partfx_query(&fx, COLOR, &color);
    sprintf_s(contents, 128, "PSLT: %i\nMAXP: %i\nTEXR: '%s'\nGRTE: %f\nCOLR: (%f, %f, %f, %f)",
              lifetime,
              maxparticles,
              texname,
              random,
              color.x, color.y, color.z, color.w
    );

    BeginScissorMode((int)view.x, (int)view.y, (int)view.width, (int)view.height);
    GuiLabel((Rectangle){15 + scroll.x, 60 + scroll.y, 340, 340}, contents);
    EndScissorMode();

    if (GuiDropdownBox(RECT(15, 30, 250, 15), options, &pickidx, picking)) {
        if (picking) {
            LoadParticleEffect(pickidx);
        }
        picking = !picking;
    }
}

void DropParticleViewer() {
    UnloadFileText(data);

    partfx_delete(&fx);
    DeleteCatalog(&cat);
}
