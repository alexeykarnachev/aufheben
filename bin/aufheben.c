#include "../src/camera.h"
#include "../src/light.h"
#include "../src/shader.h"
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
    camera.position = (Vector3){25.0, 25.0, 25.0};
    camera.projection = CAMERA_PERSPECTIVE;
    camera.target = Vector3Zero();
    camera.up = (Vector3){0.0, 1.0, 0.0};

    // Model
    Model model = LoadModel("resources/models/house.glb");
    model.materials[*model.meshMaterial].shader = material_shader;

    // Ground
    Model ground = LoadModelFromMesh(GenMeshPlane(100.0, 100.0, 2, 2));
    ground.materials[*ground.meshMaterial].shader = material_shader;
    ground.materials[*ground.meshMaterial].maps[0].color = (Color){100, 255, 90, 255};

    // Ambient lights
    int n_ambient_lights = 0;
    AmbientLight ambient_lights[16] = {0};
    ambient_lights[n_ambient_lights++] = (AmbientLight){WHITE, 0.1};

    // Directional lights
    int n_directional_lights = 0;
    DirectionalLight directional_lights[16] = {0};
    directional_lights[n_directional_lights++] = (DirectionalLight
    ){GOLD, 1.0, (Vector3){1.0, -1.0, -1.0}};

    // Point lights
    int n_point_lights = 0;
    PointLight point_lights[16] = {0};
    point_lights[n_point_lights++] = (PointLight
    ){RED, 100.0, (Vector3){10.0, 30.0, 0.0}, ATTENUATION_32};
    point_lights[n_point_lights++] = (PointLight
    ){BLUE, 100.0, (Vector3){-20.0, 30.0, 0.0}, ATTENUATION_32};

    while (!WindowShouldClose()) {
        update_free_orbit_camera(&camera);

        BeginDrawing();
        ClearBackground(DARKBLUE);

        BeginMode3D(camera);

        set_ambient_lights_shader_value(
            material_shader, ambient_lights, n_ambient_lights
        );
        set_directional_lights_shader_value(
            material_shader, directional_lights, n_directional_lights
        );
        set_point_lights_shader_value(material_shader, point_lights, n_point_lights);

        DrawModel(ground, Vector3Zero(), 1.0, WHITE);
        DrawModel(model, (Vector3){0.0, 0.0, 0.0}, 1.0, WHITE);
        DrawModel(model, (Vector3){0.0, 0.0, 15.0}, 1.0, WHITE);
        DrawModel(model, (Vector3){0.0, 0.0, -15.0}, 1.0, WHITE);
        DrawModel(model, (Vector3){15.0, 0.0, 0.0}, 1.0, WHITE);
        DrawModel(model, (Vector3){15.0, 0.0, 15.0}, 1.0, WHITE);
        DrawModel(model, (Vector3){15.0, 0.0, -15.0}, 1.0, WHITE);
        EndMode3D();

        EndDrawing();
    }
}
