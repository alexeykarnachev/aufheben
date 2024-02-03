#include "../src/shader.h"
#include "../src/camera.h"

#include "raylib.h"
#include "raymath.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

int main(void) {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Aufheben");
    SetTargetFPS(60);

    // Shader
    Shader material_shader = load_shader(0, "material.frag");

    // Camera
    Camera3D camera = {0};
    camera.fovy = 70.0;
    camera.position = (Vector3){15.0, 15.0, 15.0};
    camera.projection = CAMERA_PERSPECTIVE;
    camera.target = Vector3Zero();
    camera.up = (Vector3){0.0, 1.0, 0.0};

    // Model
    Model model = LoadModel("resources/models/house.glb");
    model.materials[*model.meshMaterial].shader = material_shader;

    while (!WindowShouldClose()) {
        update_free_orbit_camera(&camera);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);
        DrawModel(model, Vector3Zero(), 1.0, WHITE);
        EndMode3D();

        EndDrawing();
    }
}
