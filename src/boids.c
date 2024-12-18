#include "boids.h"

#include <stdlib.h>
#include "raygui.h"
#include "rlgl.h"
#include "raylib.h"
#include "raymath.h"
#include "TracyC.h"

#define RECT(x, y, w, h) ((Rectangle){x, y, w, h})
#define NUM_BOIDS 250
#define BOIDS_SIDES 4
#define BOIDS_START_RAD 0.15f
#define BOIDS_HEIGHT 0.5f

#define CENTER_FACTOR 0.004f // Rule 1
#define AVOID_FACTOR 0.05f    // Rule 2
#define MATCH_FACTOR 0.05f    // Rule 3
#define TURN_FACTOR 0.1f
#define MAX_SPEED 0.2f

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

Mesh boidMesh;
Material boidMat;

Mesh testMesh;
Material testMat;


size_t count = 50;
int *arr = NULL;

typedef enum {
    DM_DEFAULT,
    DM_DEBUG
} DrawMode;
DrawMode drawmode = DM_DEFAULT;

Mesh createBoidMesh() {
    static const int TRIS = 4;
    static const float verts[] = {
         0.00, 0.00,  0.15,
        -0.15, 0.00, -0.15,
         0.15, 0.00, -0.15,

         0.00, 0.00,  0.15,
         0.00, 0.70,  0.00,
        -0.15, 0.00, -0.15,

         0.15, 0.00, -0.15,
         0.00, 0.70,  0.00,
         0.00, 0.00,  0.15,

        -0.15, 0.00, -0.15,
         0.00, 0.70,  0.00,
         0.15, 0.00, -0.15,
    };

    Mesh m = { 0 };
    m.vertices = RL_MALLOC(TRIS * 3 * 3 * sizeof(float));
    m.texcoords = RL_MALLOC(TRIS * 3 * 2 * sizeof(float));
    m.normals = RL_MALLOC(TRIS * 3 * 3 * sizeof(float));

    m.vertexCount = TRIS * 3;
    m.triangleCount = TRIS;

    for (int i = 0; i < m.vertexCount; i++) {
        m.vertices[i * 3 + 0] = verts[i * 3 + 0];
        m.vertices[i * 3 + 1] = verts[i * 3 + 1];
        m.vertices[i * 3 + 2] = verts[i * 3 + 2];

        m.normals[i * 3 + 0] = 0.f;
        m.normals[i * 3 + 1] = 0.f;
        m.normals[i * 3 + 2] = 0.f;

        m.texcoords[i * 2 + 0] = 0.f;
        m.texcoords[i * 2 + 1] = 0.f;
    }
    UploadMesh(&m, false);

    return m;
}

//TODO: Setup grid for faster distance lookups
// Recreate grid each frame? limit the amount of other boids to check
Vector3 calcCenterRule(size_t idx) {
    TracyCZoneN(ctx, "Calc Center Rule", 1);

    Vector3 total = { 0.f, 0.f, 0.f };

    for (size_t i = 0; i < NUM_BOIDS; ++i) {
        if (i == idx) continue;
        total = Vector3Add(total, boids.pos[i]);
    }
    total = Vector3Scale(total, 1.f / (NUM_BOIDS - 1));

    TracyCZoneEnd(ctx);
    return Vector3Scale(Vector3Subtract(total, boids.pos[idx]), CENTER_FACTOR);
}

Vector3 calcDistRule(size_t idx) {
    TracyCZoneN(ctx, "Calc Dist Rule", 1);

    Vector3 total = { 0.f, 0.f, 0.f };
    for (size_t i = 0; i < NUM_BOIDS; ++i) {
        if (i == idx) continue;

        Vector3 diff = Vector3Subtract(boids.pos[idx], boids.pos[i]);
        if (Vector3Length(diff) < 1.f) {
            total = Vector3Add(total, diff);
        }
    }

    TracyCZoneEnd(ctx);
    return Vector3Scale(total, AVOID_FACTOR);
}

Vector3 calcMatchRule(size_t idx) {
    TracyCZoneN(ctx, "Calc Match Rule", 1);

    Vector3 total = { 0.f, 0.f, 0.f };

    for (size_t i = 0; i < NUM_BOIDS; ++i) {
        if (i == idx) continue;
        total = Vector3Add(total, boids.vel[i]);
    }
    total = Vector3Scale(total, 1.f / (NUM_BOIDS - 1));

    TracyCZoneEnd(ctx);
    return Vector3Scale(Vector3Subtract(total, boids.vel[idx]), MATCH_FACTOR);
}

void applyBoundRule(size_t idx) {
    TracyCZoneN(ctx, "Apply Bound Rule", 1);

    Vector3 *p = &boids.pos[idx];
    Vector3 *v = &boids.vel[idx];

    if (p->x < -boundSize.x/2.f) {
        v->x += TURN_FACTOR;
    }
    if (p->x > boundSize.x/2.f) {
        v->x -= TURN_FACTOR;
    }

    if (p->y < -boundSize.y/2.f) {
        v->y += TURN_FACTOR;
    }
    if (p->y > boundSize.x/2.f) {
        v->y -= TURN_FACTOR;
    }

    if (p->z < -boundSize.y/2.f) {
        v->z += TURN_FACTOR;
    }
    if (p->z > boundSize.x/2.f) {
        v->z -= TURN_FACTOR;
    }

    TracyCZoneEnd(ctx);
}

void applySpeedRule(size_t idx) {
    TracyCZoneN(ctx, "Apply Speed Rule", 1);

    float len = Vector3Length(boids.vel[idx]);

    if (len > MAX_SPEED) {
        boids.vel[idx] = Vector3Scale(Vector3Normalize(boids.vel[idx]), MAX_SPEED);
    }

    TracyCZoneEnd(ctx);
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

    Shader prog = LoadShader(".\\shaders\\instance-vertex.glsl", ".\\shaders\\fragment.glsl");
    prog.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(prog, "mvp");
    prog.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocationAttrib(prog, "instanceTransform");

    boidMat = LoadMaterialDefault();
    boidMat.shader = prog;
    boidMesh = GenMeshCone(BOIDS_START_RAD, BOIDS_HEIGHT, BOIDS_SIDES);

    testMesh = createBoidMesh();
    testMat = LoadMaterialDefault();

    for (size_t i = 0; i < NUM_BOIDS; ++i) {
        float rx = rand_float(-boundSize.x/2, boundSize.x/2);
        float ry = rand_float(-boundSize.y/2, boundSize.y/2);
        float rz = rand_float(-boundSize.z/2, boundSize.z/2);
        boids.pos[i] = (Vector3) { rx, ry, rz };

        rx = rand_float(-0.75f, 0.75f);
        ry = rand_float(-0.75f, 0.75f);
        rz = rand_float(-0.75f, 0.75f);
        boids.vel[i] = (Vector3) { rx, ry, rz };
    }
    print_boids(boids);

    arr = calloc(count, sizeof(int));
    TracyCAlloc(arr, count * sizeof(int));
}

void TickBoidsApp() {
    TracyCZoneN(uctx, "Update Camera", 1);
    UpdateCamera(&camera, CAMERA_THIRD_PERSON);
    TracyCZoneEnd(uctx);

    if (IsKeyPressed(KEY_R)) {
        //TODO: Handle mode draw modes later?
        if (drawmode == DM_DEFAULT) drawmode = DM_DEBUG;
        else if (drawmode == DM_DEBUG) drawmode = DM_DEFAULT;
    }

    TracyCZoneN(bctx, "Tick Boids Positions", 1);
    for (size_t i = 0; i < NUM_BOIDS; ++i) {
        //TODO: Look into other implementations, the movements are to rigid in this one
        boids.vel[i] = Vector3Add(boids.vel[i], calcCenterRule(i));
        boids.vel[i] = Vector3Add(boids.vel[i], calcDistRule(i));
        boids.vel[i] = Vector3Add(boids.vel[i], calcMatchRule(i));
        applyBoundRule(i);
        applySpeedRule(i);

        boids.pos[i] = Vector3Add(boids.pos[i], boids.vel[i]);
    }
    TracyCZoneEnd(bctx);
}

void DrawBoidsApp() {
    GuiStatusBar(RECT(0, 585, 800, 15), TextFormat("simulating %zu boids; %zu FPS", NUM_BOIDS, GetFPS()));

    TracyCZoneN(ctx, "Draw 3D", 1);
    BeginMode3D(camera);

    Matrix instances[NUM_BOIDS];
    for (size_t i = 0; i < NUM_BOIDS; ++i) {
        //TODO: Need to rework how debug works. Might need to only draw debug for boid at index 0
        //if (drawmode == DM_DEBUG) {
        //    DrawLine3D(boids.pos[i], Vector3Add(boids.pos[i], Vector3Scale(boids.vel[i], 10.f)), RED);
        //}
        Vector3 pos = boids.pos[i];
        Vector3 vel = boids.vel[i];

        Vector3 cross = Vector3Normalize(Vector3CrossProduct(vel, camUp));
        instances[i] = MatrixRotate(cross, -Vector3Angle(vel, camUp));
        instances[i] = MatrixMultiply(instances[i], MatrixTranslate(pos.x, pos.y, pos.z));
    }
    boidMat.maps[MATERIAL_MAP_DIFFUSE].color = BLUE;
    DrawMeshInstanced(testMesh, boidMat, instances, NUM_BOIDS);
    rlEnableWireMode();
    boidMat.maps[MATERIAL_MAP_DIFFUSE].color = BLACK;
    DrawMeshInstanced(testMesh, boidMat, instances, NUM_BOIDS);
    rlDisableWireMode();

    //Vector3 pos = boids.pos[0];
    //Vector3 vel = boids.vel[0];
    //Vector3 cross = Vector3Normalize(Vector3CrossProduct(vel, camUp));
    //Matrix t = MatrixRotate(cross, -Vector3Angle(vel, camUp));
    //t = MatrixMultiply(t, MatrixTranslate(pos.x, pos.y, pos.z));
    //DrawMesh(testMesh, testMat, t);
    //testMat.maps[MATERIAL_MAP_DIFFUSE].color = RED;
    //DrawMesh(testMesh, testMat, t);
    //rlEnableWireMode();
    //testMat.maps[MATERIAL_MAP_DIFFUSE].color = BLACK;
    //DrawMesh(testMesh, testMat, t);
    //rlDisableWireMode();

    // Bounds
    DrawCubeWiresV(camTarget, boundSize, GREEN);
    DrawCubeV(camTarget, boundSize, ColorAlpha(GREEN, 0.15f));
    EndMode3D();
    TracyCZoneEnd(ctx);

    if (drawmode == DM_DEBUG) {
        for (size_t i = 0; i < NUM_BOIDS; ++i) {
            Vector2 spos = GetWorldToScreen(boids.pos[i], camera);
            float speed = Vector3Length(boids.vel[i]);

            const char *txt = TextFormat("%f", speed);
            DrawText(txt, (int)spos.x, (int)spos.y, 10, WHITE);
        }
    }
}

void DropBoidsApp() {
    TraceLog(LOG_DEBUG, "[BOIDS] dropping boids viewer application");
    UnloadMesh(testMesh);

    UnloadMaterial(boidMat);
    UnloadMesh(boidMesh);

    TracyCFree(arr);
    free(arr);
    arr = NULL;
}
