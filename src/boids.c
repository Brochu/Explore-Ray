#include "boids.h"

#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "raygui.h"
#include "raymath.h"

#define RECT(x, y, w, h) ((Rectangle){x, y, w, h})
#define NUM_BOIDS 1

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
    //TODO: Find a way to draw a pyramid
    DrawCube(pos, 0.1f, 0.1f, 0.1f, BLUE);
    DrawLine3D(pos, Vector3Add(pos, dir), RED);
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
        float rx = (float)rand() / RAND_MAX;
        float ry = (float)rand() / RAND_MAX;
        float rz = (float)rand() / RAND_MAX;
        boids.pos[i] = (Vector3) { rx, ry, rz };

        rx = (float)rand() / RAND_MAX;
        ry = (float)rand() / RAND_MAX;
        rz = (float)rand() / RAND_MAX;
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
