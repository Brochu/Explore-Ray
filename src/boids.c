#include "boids.h"

#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "raygui.h"
#include "raymath.h"

#define RECT(x, y, w, h) ((Rectangle){x, y, w, h})
#define NUM_BOIDS 100

float rand_float(float lo, float hi) {
    return lo + ((float)rand() / RAND_MAX) * (hi - lo);
}

typedef struct { 
    Vector3 pos[NUM_BOIDS];
    Vector3 fwd[NUM_BOIDS];
} boids_state;
boids_state boids;

void print_boids(boids_state state) {
    for (size_t i = 0; i < NUM_BOIDS; ++i) {
        Vector3 p = state.pos[i];
        Vector3 f = state.fwd[i];
        TraceLog(LOG_DEBUG, "[BOIDS][%zu] pos: (%f, %f, %f); fwd: (%f, %f, %f)", i, p.x, p.y, p.z, f.x, f.y, f.z);
    }
}

Vector3 boundSize = { 15.f, 10.f, 10.f };

Camera3D camera;
Vector3 camPos = { 0.f, 5.f, 20.f };
Vector3 camTarget = { 0.f, 0.f, 0.f };
Vector3 camUp = { 0.f, 1.f, 0.f };

void drawBounds(Vector3 pos, Vector3 size) {
    DrawCubeWiresV(pos, size, GREEN);
    DrawCubeV(pos, size, ColorAlpha(GREEN, 0.15f));
}

void drawBoid(Vector3 pos, Vector3 dir) {
    Vector3 ndir = Vector3Scale(Vector3Normalize(dir), 0.75f);
    Vector3 up = { 0.f, 0.5f, 0.f };
    Vector3 left = Vector3Scale(Vector3CrossProduct(ndir, up), 0.5f);
    up = Vector3CrossProduct(ndir, left);

    Vector3 p0 = Vector3Add(pos, up);
    Vector3 p1 = Vector3Add(pos, Vector3Subtract(left, up));
    Vector3 p2 = Vector3Subtract(pos, Vector3Add(left, up));
    Vector3 apex = Vector3Add(pos, ndir);

    DrawLine3D(p0, p1, BLUE);
    DrawLine3D(p0, p2, BLUE);
    DrawLine3D(p0, apex, BLUE);
    DrawLine3D(p1, apex, BLUE);
    DrawLine3D(p2, apex, BLUE);

    DrawTriangle3D(p0, p1, p2, ColorAlpha(BLUE, 0.75f));
    DrawTriangle3D(p0, p1, p2, ColorAlpha(BLUE, 0.75f));
    DrawTriangle3D(p0, apex, p1, ColorAlpha(BLUE, 0.75f));
    DrawTriangle3D(p2, apex, p0, ColorAlpha(BLUE, 0.75f));
    DrawTriangle3D(p1, apex, p2, ColorAlpha(BLUE, 0.75f));

    Ray r = { .position = pos, .direction = dir };
    DrawRay(r, RED);
}

void InitBoidsApp() {
    SetTraceLogLevel(LOG_DEBUG);
    TraceLog(LOG_DEBUG, "[BOIDS] Starting boids viewer application");
    DisableCursor();

    camera = (Camera3D) {
        .position = camPos,
        .target = camTarget,
        .up = camUp,
        .fovy = 45.f,
        .projection = CAMERA_PERSPECTIVE
    };

    for (size_t i = 0; i < NUM_BOIDS; ++i) {
        float rx = rand_float(-boundSize.x/2, boundSize.x/2);
        float ry = rand_float(-boundSize.y/2, boundSize.y/2);
        float rz = rand_float(-boundSize.z/2, boundSize.z/2);
        boids.pos[i] = (Vector3) { rx, ry, rz };

        rx = rand_float(-1.f, 1.f);
        ry = rand_float(-1.f, 1.f);
        rz = rand_float(-1.f, 1.f);
        boids.fwd[i] = (Vector3) { rx, ry, rz };
    }
    print_boids(boids);
}

void TickBoidsApp() {
    UpdateCamera(&camera, CAMERA_THIRD_PERSON);
    //TODO: Step movements for boids
}

void DrawBoidsApp() {
    GuiStatusBar(RECT(0, 585, 800, 15), TextFormat("simulating %zu boids", NUM_BOIDS));

    BeginMode3D(camera);
    for (size_t i = 0; i < NUM_BOIDS; ++i) {
        drawBoid(boids.pos[i], boids.fwd[i]);
    }

    drawBounds(camTarget, boundSize);
    EndMode3D();
}

void DropBoidsApp() {
    printf("[BOIDS] dropping boids viewer application\n");
}
