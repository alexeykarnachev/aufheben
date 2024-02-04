#include "../src/camera.h"
#include "../src/lights.h"
#include "../src/shader.h"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <math.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

static Model HOUSE;
static Model GROUND;

static void draw_scene(void);

int main(void) {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Aufheben");
    SetTargetFPS(60);

    // Shader
    Shader material_shader = load_shader(0, "material.frag");

    // Camera
    Camera3D camera = {0};
    camera.fovy = 70.0;
    camera.position = (Vector3){25.0, 25.0, 25.0};
    camera.projection = CAMERA_PERSPECTIVE;
    camera.target = Vector3Zero();
    camera.up = (Vector3){0.0, 1.0, 0.0};

    // Model
    HOUSE = LoadModel("resources/models/house.glb");
    HOUSE.materials[*HOUSE.meshMaterial].shader = material_shader;

    // Ground
    GROUND = LoadModelFromMesh(GenMeshPlane(100.0, 100.0, 2, 2));
    GROUND.materials[*GROUND.meshMaterial].shader = material_shader;
    GROUND.materials[*GROUND.meshMaterial].maps[0].color = (Color){100, 255, 90, 255};

    // -------------------------------------------------------------------
    // Lighting
    load_lights(512);

    add_ambient_light(WHITE, 0.1);
    // add_point_light(RED, 100.0, (Vector3){10.0, 30.0, 0.0}, ATTENUATION_32);
    // add_point_light(BLUE, 100.0, (Vector3){-20.0, 30.0, 0.0}, ATTENUATION_32);

    while (!WindowShouldClose()) {
        clear_directional_lights();
        float t = GetTime() * 0.1;
        add_directional_light(GOLD, 1.0, (Vector3){sinf(t), -1.0, cosf(t)});
        add_directional_light(GOLD, 1.0, (Vector3){cosf(t), -1.0, sinf(t)});

        update_free_orbit_camera(&camera);
        draw_shadow_maps(draw_scene);

        BeginDrawing();
        rlViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        rlEnableBackfaceCulling();
        rlSetCullFace(RL_CULL_FACE_BACK);
        ClearBackground(DARKBLUE);

        BeginMode3D(camera);

        set_lights_shader_values(material_shader);

        draw_scene();

        EndMode3D();

        DrawFPS(10, 10);

        EndDrawing();
    }
}

static void draw_scene(void) {
    DrawModel(GROUND, Vector3Zero(), 1.0, WHITE);
    DrawModel(HOUSE, (Vector3){0.0, 0.0, 0.0}, 1.0, WHITE);
    DrawModel(HOUSE, (Vector3){0.0, 0.0, 15.0}, 1.0, WHITE);
    DrawModel(HOUSE, (Vector3){10.0, 0.0, 0.0}, 1.0, WHITE);
    DrawModel(HOUSE, (Vector3){10.0, 0.0, 15.0}, 1.0, WHITE);
}
