#include "partviewer.h"
#include "catalog.h"
#include "partfx.h"
#include "raygui.h"
#include "raylib.h"

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

Camera3D camera = { 0 };
Texture texture = { 0 };

void LoadParticleEffect(int idx) {
    if (data != NULL) {
        UnloadFileText(data);
        partfx_reset(&fx);
    }

    char path[128];
    sprintf_s(path, 128, "%s/%s", FOLDER, cat.paths[pickidx]);
    data = LoadFileText(path);

    if (!partfx_parse(&fx, data, strlen(data))) {
        printf("[ERROR] Failed to parse particle effect\n");
        return;
    }

    printf("[FX] Successfully parsed: %s\n", cat.paths[pickidx]);

    // Print tree for debugging
    if (fx.root != NULL) {
        partfx_print_tree(fx.root, 0);
    }

    partfx_node_t *texr = partfx_get(fx.root, "TEXR");
    if (texr != NULL) {
        partfx_node_t *cnst = partfx_get(texr, "CNST");
        if (cnst != NULL) {
            partfx_node_t *tex = partfx_get(cnst, "tex");
            if (tex != NULL && tex->type == NODE_STRING) {
                char tex_path[128];
                sprintf_s(tex_path, 128, "%s/%s", FOLDER, tex->data.as_string + 13);
                printf(" -=-=-= '%s'\n", tex_path);

                Image test = LoadImage(tex_path);
                texture = LoadTextureFromImage(test);
            }
        }
    }

    partfx_node_t *sub_part = partfx_get(fx.root, "KSSM");
    if (sub_part != NULL) {
        partfx_node_t *end_frame = partfx_get(sub_part, "endFrame");
        if (end_frame != NULL) {
            fx.num_frames = end_frame->data.as_int;
            fx.p = 0;
            fx.t = 0.f;
        }
    }
}

void InitParticleViewer() {
    camera.position = (Vector3){ 10.f, 10.f, 10.f };
    camera.target = (Vector3){ 0.f, 0.f, 0.f };
    camera.up = (Vector3){ 0.f, 1.f, 0.f };
    camera.fovy = 45.f;

    ParseCatalog(&cat);
    partfx_init(&fx);

    options = TextJoin(cat.names, (int)cat.size, ";");
    LoadParticleEffect(pickidx);
}

void DrawParticleViewer() {
    GuiLabel(RECT(0, 0, 800, 15), "MP1 - Particle System Viewer");
    GuiStatusBar(RECT(0, 585, 800, 15), TextFormat("current file: %s", cat.paths[pickidx]));

    GuiScrollPanel(RECT(15, 60, 775, 450), NULL, RECT(0, 0, 340, 600), &scroll, &view);

    // Extract particle data from the tree
    char contents[2048];
    contents[0] = '\0';

    if (fx.root != NULL) {
        // Get DNA type
        const char *dna_type = partfx_get_string(fx.root, "DNAType", "UNKNOWN");

        char line[128];
        sprintf_s(line, sizeof(line), "Type: %s\n\n", dna_type);
        strcat_s(contents, sizeof(contents), line);

        sprintf_s(line, sizeof(line), "Time: p = %lli / %lli; t = %f\n\n", fx.p, fx.num_frames, fx.t);
        strcat_s(contents, sizeof(contents), line);

        // Try to extract common GPSM properties
        if (strcmp(dna_type, "GPSM<UniqueID32>") == 0) {
            int maxp = partfx_get_int(fx.root, "MAXP", -1);
            float grte = partfx_get_float(fx.root, "GRTE", -1.0f);
            int ltme = partfx_get_int(fx.root, "LTME", -1);
            float size = partfx_get_float(fx.root, "SIZE", -1.0f);

            sprintf_s(line, sizeof(line), "MAXP: %d\n", maxp);
            strcat_s(contents, sizeof(contents), line);

            sprintf_s(line, sizeof(line), "GRTE: %.2f\n", grte);
            strcat_s(contents, sizeof(contents), line);

            sprintf_s(line, sizeof(line), "LTME: %d\n", ltme);
            strcat_s(contents, sizeof(contents), line);

            sprintf_s(line, sizeof(line), "SIZE: %.2f\n\n", size);
            strcat_s(contents, sizeof(contents), line);

            // Get texture
            partfx_node_t *texr = partfx_get(fx.root, "TEXR");
            if (texr != NULL) {
                partfx_node_t *cnst = partfx_get(texr, "CNST");
                if (cnst != NULL) {
                    partfx_node_t *tex = partfx_get(cnst, "tex");
                    if (tex != NULL && tex->type == NODE_STRING) {
                        sprintf_s(line, sizeof(line), "Texture: %s\n\n", tex->data.as_string);
                        strcat_s(contents, sizeof(contents), line);
                    }
                }
            }

            // Get model
            const char *model = partfx_get_string(fx.root, "PMDL", NULL);
            if (model != NULL) {
                sprintf_s(line, sizeof(line), "Model: %s\n\n", model);
                strcat_s(contents, sizeof(contents), line);
            }

            // Check for color animation
            partfx_node_t *pmcl = partfx_get(fx.root, "PMCL");
            if (pmcl != NULL) {
                partfx_node_t *keye = partfx_get(pmcl, "KEYE");
                if (keye != NULL) {
                    partfx_node_t *keys = partfx_get(keye, "keys");
                    if (keys != NULL && keys->type == NODE_SEQUENCE) {
                        size_t key_count = partfx_sequence_length(keys);
                        sprintf_s(line, sizeof(line), "Color Keys: %zu\n", key_count);
                        strcat_s(contents, sizeof(contents), line);

                        // Show first color
                        if (key_count > 0) {
                            partfx_node_t *first = partfx_sequence_get(keys, 0);
                            if (first != NULL && first->type == NODE_SEQUENCE) {
                                size_t components = partfx_sequence_length(first);
                                if (components == 4) {
                                    partfx_node_t *r = partfx_sequence_get(first, 0);
                                    partfx_node_t *g = partfx_sequence_get(first, 1);
                                    partfx_node_t *b = partfx_sequence_get(first, 2);
                                    partfx_node_t *a = partfx_sequence_get(first, 3);

                                    if (r && g && b && a) {
                                        float rf = (r->type == NODE_FLOAT) ? r->data.as_float : (float)r->data.as_int;
                                        float gf = (g->type == NODE_FLOAT) ? g->data.as_float : (float)g->data.as_int;
                                        float bf = (b->type == NODE_FLOAT) ? b->data.as_float : (float)b->data.as_int;
                                        float af = (a->type == NODE_FLOAT) ? a->data.as_float : (float)a->data.as_int;

                                        sprintf_s(line, sizeof(line), "First Color: (%.2f, %.2f, %.2f, %.2f)\n", 
                                                  rf, gf, bf, af);
                                        strcat_s(contents, sizeof(contents), line);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        // Try to extract WPSM properties
        else if (strcmp(dna_type, "WPSM<UniqueID32>") == 0) {
            int pslt = partfx_get_int(fx.root, "PSLT", -1);
            float trat = partfx_get_float(fx.root, "TRAT", -1.0f);

            sprintf_s(line, sizeof(line), "PSLT: 0x%08X\n", pslt);
            strcat_s(contents, sizeof(contents), line);

            sprintf_s(line, sizeof(line), "TRAT: %.2f\n\n", trat);
            strcat_s(contents, sizeof(contents), line);

            // Get attached particle system
            const char *apsm = partfx_get_string(fx.root, "APSM", NULL);
            if (apsm != NULL) {
                sprintf_s(line, sizeof(line), "Attached System:\n  %s\n\n", apsm);
                strcat_s(contents, sizeof(contents), line);
            }

            // Get collision response
            const char *colr = partfx_get_string(fx.root, "COLR", NULL);
            if (colr != NULL) {
                sprintf_s(line, sizeof(line), "Collision Response:\n  %s\n", colr);
                strcat_s(contents, sizeof(contents), line);
            }
        }
    } else {
        strcpy_s(contents, sizeof(contents), "No data loaded or parse failed.");
    }

    BeginScissorMode((int)view.x, (int)view.y, (int)view.width, (int)view.height);
    GuiLabel((Rectangle){15 + scroll.x, 60 + scroll.y, 340, 600}, contents);
    EndScissorMode();

    BeginMode3D(camera);
    DrawBillboard(camera, texture, (Vector3) { 0.f, 0.f, 0.f }, 5.f, WHITE);
    EndMode3D();

    if (GuiDropdownBox(RECT(15, 30, 250, 15), options, &pickidx, picking)) {
        if (picking) {
            LoadParticleEffect(pickidx);
        }
        picking = !picking;
    }

    fx.p = (fx.p+1) % fx.num_frames;
    fx.t = (float)fx.p / fx.num_frames;
}

void DropParticleViewer() {
    UnloadFileText(data);
    partfx_delete(&fx);
    DeleteCatalog(&cat);
}
