#include "../src/camera.h"
#include "../src/shader.h"
#include "raylib.h"
#include "raymath.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

typedef struct AmbientLight {
    Color color;
    float intensity;
} AmbientLight;

typedef struct DirectionalLight {
    Color color;
    float intensity;
    Vector3 direction;
} DirectionalLight;

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

    // Ambient light
    AmbientLight ambient_light = {0};
    ambient_light.color = WHITE;
    ambient_light.intensity = 0.3;

    // Directional lights
    int n_directional_lights = 0;
    DirectionalLight directional_lights[4] = {0};
    directional_lights[n_directional_lights++] = (DirectionalLight
    ){GOLD, 1.0, (Vector3){1.0, -1.0, -1.0}};

    while (!WindowShouldClose()) {
        update_free_orbit_camera(&camera);

        BeginDrawing();
        ClearBackground(DARKBLUE);

        BeginMode3D(camera);
        Shader shader = model.materials[*model.meshMaterial].shader;

        Vector4 ambient_light_color = ColorNormalize(ambient_light.color);
        SetShaderValueV(
            shader,
            GetShaderLocation(shader, "ambient_light.color"),
            &ambient_light_color,
            SHADER_UNIFORM_VEC3,
            1
        );
        SetShaderValue(
            shader,
            GetShaderLocation(shader, "ambient_light.intensity"),
            &ambient_light.intensity,
            SHADER_UNIFORM_FLOAT
        );

        for (int i = 0; i < n_directional_lights; ++i) {
            DirectionalLight directional_light = directional_lights[i];
            Vector4 directional_light_color = ColorNormalize(directional_light.color);
            SetShaderValueV(
                shader,
                GetShaderLocation(shader, TextFormat("directional_lights[%d].color", i)),
                &directional_light_color,
                SHADER_UNIFORM_VEC3,
                1
            );
            SetShaderValue(
                shader,
                GetShaderLocation(
                    shader, TextFormat("directional_lights[%d].intensity", i)
                ),
                &directional_light.intensity,
                SHADER_UNIFORM_FLOAT
            );
            SetShaderValueV(
                shader,
                GetShaderLocation(
                    shader, TextFormat("directional_lights[%d].direction", i)
                ),
                &directional_light.direction,
                SHADER_UNIFORM_VEC3,
                1
            );
        }
        SetShaderValue(
            shader,
            GetShaderLocation(shader, "n_directional_lights"),
            &n_directional_lights,
            SHADER_UNIFORM_INT
        );

        DrawModel(model, Vector3Zero(), 1.0, WHITE);
        EndMode3D();

        EndDrawing();
    }
}
