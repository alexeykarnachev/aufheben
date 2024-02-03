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

typedef struct PointLight {
    Color color;
    float intensity;
    Vector3 position;
    Vector3 attenuation;
} PointLight;

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

    // Ground
    Model ground = LoadModelFromMesh(GenMeshPlane(100.0, 100.0, 2, 2));
    ground.materials[*ground.meshMaterial].shader = material_shader;
    ground.materials[*ground.meshMaterial].maps[0].color = (Color){100, 255, 90, 255};

    // Ambient light
    AmbientLight ambient_light = {0};
    ambient_light.color = WHITE;
    ambient_light.intensity = 0.1;

    // Directional lights
    int n_directional_lights = 0;
    DirectionalLight directional_lights[16] = {0};
    directional_lights[n_directional_lights++] = (DirectionalLight
    ){GOLD, 1.0, (Vector3){1.0, -1.0, -1.0}};

    // Point lights
    int n_point_lights = 0;
    PointLight point_lights[16] = {0};
    point_lights[n_point_lights++] = (PointLight
    ){RED, 10.0, (Vector3){10.0, 10.0, 0.0}, (Vector3){1.0, 0.045, 0.0075}};

    while (!WindowShouldClose()) {
        update_free_orbit_camera(&camera);

        BeginDrawing();
        ClearBackground(DARKBLUE);

        BeginMode3D(camera);

        Vector4 ambient_light_color = ColorNormalize(ambient_light.color);
        SetShaderValueV(
            material_shader,
            GetShaderLocation(material_shader, "ambient_light.color"),
            &ambient_light_color,
            SHADER_UNIFORM_VEC3,
            1
        );
        SetShaderValue(
            material_shader,
            GetShaderLocation(material_shader, "ambient_light.intensity"),
            &ambient_light.intensity,
            SHADER_UNIFORM_FLOAT
        );

        for (int i = 0; i < n_directional_lights; ++i) {
            DirectionalLight light = directional_lights[i];
            Vector4 color = ColorNormalize(light.color);
            SetShaderValueV(
                material_shader,
                GetShaderLocation(
                    material_shader, TextFormat("directional_lights[%d].color", i)
                ),
                &color,
                SHADER_UNIFORM_VEC3,
                1
            );
            SetShaderValue(
                material_shader,
                GetShaderLocation(
                    material_shader, TextFormat("directional_lights[%d].intensity", i)
                ),
                &light.intensity,
                SHADER_UNIFORM_FLOAT
            );
            SetShaderValueV(
                material_shader,
                GetShaderLocation(
                    material_shader, TextFormat("directional_lights[%d].direction", i)
                ),
                &light.direction,
                SHADER_UNIFORM_VEC3,
                1
            );
        }
        SetShaderValue(
            material_shader,
            GetShaderLocation(material_shader, "n_directional_lights"),
            &n_directional_lights,
            SHADER_UNIFORM_INT
        );

        for (int i = 0; i < n_point_lights; ++i) {
            PointLight light = point_lights[i];
            Vector4 color = ColorNormalize(light.color);
            SetShaderValueV(
                material_shader,
                GetShaderLocation(
                    material_shader, TextFormat("point_lights[%d].color", i)
                ),
                &color,
                SHADER_UNIFORM_VEC3,
                1
            );
            SetShaderValue(
                material_shader,
                GetShaderLocation(
                    material_shader, TextFormat("point_lights[%d].intensity", i)
                ),
                &light.intensity,
                SHADER_UNIFORM_FLOAT
            );
            SetShaderValueV(
                material_shader,
                GetShaderLocation(
                    material_shader, TextFormat("point_lights[%d].position", i)
                ),
                &light.position,
                SHADER_UNIFORM_VEC3,
                1
            );
            SetShaderValueV(
                material_shader,
                GetShaderLocation(
                    material_shader, TextFormat("point_lights[%d].attenuation", i)
                ),
                &light.attenuation,
                SHADER_UNIFORM_VEC3,
                1
            );
        }
        SetShaderValue(
            material_shader,
            GetShaderLocation(material_shader, "n_point_lights"),
            &n_point_lights,
            SHADER_UNIFORM_INT
        );

        DrawModel(ground, Vector3Zero(), 1.0, WHITE);
        DrawModel(model, Vector3Zero(), 1.0, WHITE);
        EndMode3D();

        EndDrawing();
    }
}
