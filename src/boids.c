#include "boids.h"

#include <stdlib.h>
#include "raylib.h"
#include "raygui.h"
#include "raymath.h"

#define RECT(x, y, w, h) ((Rectangle){x, y, w, h})
#define NUM_BOIDS 15
#define BOIDS_SIDES 4
#define BOIDS_START_RAD 0.2f
#define BOIDS_END_RAD 0.f
#define BOIDS_SPEED 5.f

float rand_float(float lo, float hi) {
    return lo + ((float)rand() / RAND_MAX) * (hi - lo);
}

typedef struct { 
    Vector3 pos[NUM_BOIDS];
    Vector3 vel[NUM_BOIDS];
    float speed;
} boids_state;
boids_state boids;

void print_boids(boids_state state) {
    for (size_t i = 0; i < NUM_BOIDS; ++i) {
        Vector3 p = state.pos[i];
        Vector3 f = state.vel[i];
        TraceLog(LOG_DEBUG, "[BOIDS][%zu] pos: (%f, %f, %f); fwd: (%f, %f, %f)", i, p.x, p.y, p.z, f.x, f.y, f.z);
    }
}

Vector3 boundSize = { 15.f, 10.f, 10.f };

Camera3D camera;
Vector3 camPos = { 0.f, 3.f, 25.f };
Vector3 camTarget = { 0.f, 0.f, 0.f };
Vector3 camUp = { 0.f, 1.f, 0.f };

typedef enum {
    DM_DEFAULT,
    DM_DEBUG
} DrawMode;
DrawMode drawmode = DM_DEFAULT;

void drawBoid(Vector3 pos, Vector3 dir) {
    if (drawmode == DM_DEBUG) {
        DrawLine3D(pos, Vector3Add(pos, dir), RED);
    }

    Vector3 end = Vector3Add(pos, Vector3Scale(Vector3Normalize(dir), 0.5f));
    DrawCylinderWiresEx(pos, end, BOIDS_START_RAD, BOIDS_END_RAD, BOIDS_SIDES, BLACK);
    DrawCylinderEx(pos, end, BOIDS_START_RAD, BOIDS_END_RAD, BOIDS_SIDES, BLUE);
}

//TODO: Setup grid for faster distance lookups
// Recreate grid each frame? limit the amount of other boids to check
Vector3 rule1(int idx) {
    TraceLog(LOG_DEBUG, "[BOIDS] Calc rule 1 for boid at idx = %zu", idx);
    return (Vector3){ .x = 0.f, .y = 0.f, .z = 0.f };
}

Vector3 rule2(int idx) {
    TraceLog(LOG_DEBUG, "[BOIDS] Calc rule 2 for boid at idx = %zu", idx);
    return (Vector3){ .x = 0.f, .y = 0.f, .z = 0.f };
}

Vector3 rule3(int idx) {
    TraceLog(LOG_DEBUG, "[BOIDS] Calc rule 3 for boid at idx = %zu", idx);
    return (Vector3){ .x = 0.f, .y = 0.f, .z = 0.f };
}

Vector3 ruleBounds(int idx) {
    return (Vector3){ .x = 0.f, .y = 0.f, .z = 0.f };
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
        boids.vel[i] = Vector3Normalize((Vector3) { rx, ry, rz });

        boids.speed = BOIDS_SPEED;
    }
    print_boids(boids);
}

void TickBoidsApp() {
    if (IsKeyPressed(KEY_R)) {
        //TODO: Handle mode draw modes later?
        if (drawmode == DM_DEFAULT) drawmode = DM_DEBUG;
        else if (drawmode == DM_DEBUG) drawmode = DM_DEFAULT;
    }

    UpdateCamera(&camera, CAMERA_THIRD_PERSON);
    for (size_t i = 0; i < NUM_BOIDS; ++i) {
        Vector3 mod = Vector3Scale(boids.vel[i], boids.speed * GetFrameTime());
        boids.pos[i] = Vector3Add(boids.pos[i], mod);

        if (boids.pos[i].x < -boundSize.x/2) {
            boids.pos[i].x += boundSize.x;
        }
        else if (boids.pos[i].x > boundSize.x/2) {
            boids.pos[i].x -= boundSize.x;
        }

        if (boids.pos[i].y < -boundSize.y/2) {
            boids.pos[i].y += boundSize.y;
        }
        else if (boids.pos[i].y > boundSize.y/2) {
            boids.pos[i].y -= boundSize.y;
        }

        if (boids.pos[i].z < -boundSize.z/2) {
            boids.pos[i].z += boundSize.z;
        }
        else if (boids.pos[i].z > boundSize.z/2) {
            boids.pos[i].z -= boundSize.z;
        }
    }
}

void DrawBoidsApp() {
    GuiStatusBar(RECT(0, 585, 800, 15), TextFormat("simulating %zu boids; %zu FPS", NUM_BOIDS, GetFPS()));

    BeginMode3D(camera);
    for (size_t i = 0; i < NUM_BOIDS; ++i) {
        //TODO: Look into instanciating the meshes to render in one draw call
        drawBoid(boids.pos[i], boids.vel[i]);
    }

    // Bounds
    DrawCubeWiresV(camTarget, boundSize, GREEN);
    DrawCubeV(camTarget, boundSize, ColorAlpha(GREEN, 0.15f));
    EndMode3D();
}

void DropBoidsApp() {
    TraceLog(LOG_DEBUG, "[BOIDS] dropping boids viewer application\n");
}
