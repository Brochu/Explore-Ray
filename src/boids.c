#include "boids.h"

#include <stdio.h>
#include "raylib.h"
#include "raygui.h"

#define RECT(x, y, w, h) ((Rectangle){x, y, w, h})

Camera3D camera;
size_t numBoids = 10;

void InitBoidsApp() {
    printf("[BOIDS] Starting boids viewer application\n");

    camera.position = (Vector3){ 10.f, 10.f, 10.f };
    camera.target = (Vector3){ 0.f, 0.f, 0.f };
    camera.up = (Vector3){ 0.f, 1.f, 0.f };
    camera.fovy = 45.f;
    camera.projection = CAMERA_PERSPECTIVE; 

    DisableCursor();
}

void TickBoidsApp() {
    UpdateCamera(&camera, CAMERA_ORBITAL);
}

void DrawBoidsApp() {
    GuiStatusBar(RECT(0, 585, 800, 15), TextFormat("simulating %zu boids", numBoids));

    BeginMode3D(camera);
    Vector3 p = {0.f, 0.f, 0.f};
    DrawCube(p, 2.f, 2.f, 2.f, ColorAlpha(GREEN, 0.25f));
    DrawCubeWires(p, 2.f, 2.f, 2.f, GREEN);
    EndMode3D();
}

void DropBoidsApp() {
    printf("[BOIDS] dropping boids viewer application\n");
}
