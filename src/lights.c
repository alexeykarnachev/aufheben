#include "lights.h"

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "shader.h"
#include <stdio.h>
#include <stdlib.h>

#define SHADOW_MAP_WIDTH 3000
#define SHADOW_MAP_HEIGHT 3000

#define MAX_N_AMBIENT_LIGHTS 16
#define MAX_N_DIRECTIONAL_LIGHTS 16
#define MAX_N_POINT_LIGHTS 16

typedef struct ShadowMap {
    unsigned int fbo;
    unsigned int texture;
} ShadowMap;

typedef struct Light {
    Color color;
    float intensity;
} Light;

typedef Light AmbientLight;

typedef struct DirectionalLight {
    Light light;
    Vector3 direction;

    ShadowMap shadow_map;
    Matrix vp;  // View-projection matrix from the last shadow pass
} DirectionalLight;

typedef struct PointLight {
    Light light;
    Vector3 position;
    Vector3 attenuation;
} PointLight;

static int N_AMBIENT_LIGHTS = 0;
static int N_DIRECTIONAL_LIGHTS = 0;
static int N_POINT_LIGHTS = 0;

static AmbientLight AMBIENT_LIGHTS[MAX_N_AMBIENT_LIGHTS];
static DirectionalLight DIRECTIONAL_LIGHTS[MAX_N_DIRECTIONAL_LIGHTS];
static PointLight POINT_LIGHTS[MAX_N_POINT_LIGHTS];

static Shader SHADOW_PASS_SHADER;

void load_lights(void) {
    SHADOW_PASS_SHADER = load_shader(0, "shadow_pass.frag");

    for (int i = 0; i < MAX_N_DIRECTIONAL_LIGHTS; ++i) {
        DirectionalLight *light = &DIRECTIONAL_LIGHTS[i];
        unsigned int *fbo = &light->shadow_map.fbo;
        unsigned int *texture = &light->shadow_map.texture;

        *fbo = rlLoadFramebuffer(-1, -1);
        *texture = rlLoadTextureDepth(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, false);
        rlFramebufferAttach(
            *fbo, *texture, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_TEXTURE2D, 0
        );

        if (!rlFramebufferComplete(*fbo)) {
            TraceLog(LOG_ERROR, "Failed to create shadow map fbo");
            exit(1);
        }
    }
}

void clear_ambient_lights(void) {
    N_AMBIENT_LIGHTS = 0;
}

void clear_directional_lights(void) {
    N_DIRECTIONAL_LIGHTS = 0;
}

void clear_point_lights(void) {
    N_POINT_LIGHTS = 0;
}

void clear_lights(void) {
    clear_ambient_lights();
    clear_directional_lights();
    clear_point_lights();
}

bool add_ambient_light(Color color, float intensity) {
    if (N_AMBIENT_LIGHTS == MAX_N_AMBIENT_LIGHTS) {
        TraceLog(LOG_WARNING, "Can't add more ambient lights");
        return false;
    }

    AmbientLight *light = &AMBIENT_LIGHTS[N_AMBIENT_LIGHTS++];
    light->color = color;
    light->intensity = intensity;

    return true;
}

bool add_directional_light(Color color, float intensity, Vector3 direction) {
    if (N_DIRECTIONAL_LIGHTS == MAX_N_DIRECTIONAL_LIGHTS) {
        TraceLog(LOG_WARNING, "Can't add more directional lights");
        return false;
    }

    DirectionalLight *light = &DIRECTIONAL_LIGHTS[N_DIRECTIONAL_LIGHTS++];
    light->light.color = color;
    light->light.intensity = intensity;
    light->direction = Vector3Normalize(direction);

    return true;
}

bool add_point_light(
    Color color, float intensity, Vector3 position, Vector3 attenuation
) {
    if (N_POINT_LIGHTS == MAX_N_POINT_LIGHTS) {
        TraceLog(LOG_WARNING, "Can't add more point lights");
        return false;
    }

    PointLight *light = &POINT_LIGHTS[N_POINT_LIGHTS++];
    light->light.color = color;
    light->light.intensity = intensity;
    light->position = position;
    light->attenuation = attenuation;

    return true;
}

void draw_shadow_maps(void (*draw_scene)()) {
    rlViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
    for (int i = 0; i < N_DIRECTIONAL_LIGHTS; ++i) {
        DirectionalLight *light = &DIRECTIONAL_LIGHTS[i];
        rlBindFramebuffer(RL_DRAW_FRAMEBUFFER, light->shadow_map.fbo);
        rlClearScreenBuffers();

        Camera3D camera = {0};
        camera.projection = CAMERA_ORTHOGRAPHIC;
        camera.fovy = 75.0;
        camera.position = Vector3Scale(light->direction, -100.0);
        camera.target = Vector3Add(camera.position, light->direction);
        camera.up = (Vector3){0.0, 1.0, 0.0};

        BeginMode3D(camera);

        Matrix v = rlGetMatrixModelview();
        Matrix p = rlGetMatrixProjection();
        light->vp = MatrixMultiply(v, p);

        // rlEnableShader(SHADOW_PASS_SHADER.id);
        // BeginShaderMode(SHADOW_PASS_SHADER);
        draw_scene();
        // EndShaderMode();
        // rlDisableShader();
        EndMode3D();
    }

    rlBindFramebuffer(RL_DRAW_FRAMEBUFFER, 0);
}

static void set_light_shader_value(
    Shader shader, Light light, const char *name, int idx
) {
    const char *uniform_name = TextFormat("%s_lights[%d]", name, idx);
    Vector4 color = ColorNormalize(light.color);

    SetShaderValueV(
        shader,
        GetShaderLocation(shader, TextFormat("%s.color", uniform_name)),
        &color,
        SHADER_UNIFORM_VEC3,
        1
    );
    SetShaderValue(
        shader,
        GetShaderLocation(shader, TextFormat("%s.intensity", uniform_name)),
        &light.intensity,
        SHADER_UNIFORM_FLOAT
    );
}

static void set_n_lights_shader_value(Shader shader, int n, const char *name) {
    const char *uniform_name = TextFormat("n_%s_lights", name);
    SetShaderValue(
        shader, GetShaderLocation(shader, uniform_name), &n, SHADER_UNIFORM_INT
    );
}

void set_lights_shader_values(Shader shader) {
    // -------------------------------------------------------------------
    // Ambient lights
    const char *name = "ambient";
    for (int i = 0; i < N_AMBIENT_LIGHTS; ++i) {
        set_light_shader_value(shader, AMBIENT_LIGHTS[i], name, i);
    }
    set_n_lights_shader_value(shader, N_AMBIENT_LIGHTS, name);

    // -------------------------------------------------------------------
    // Directional lights
    name = "directional";
    for (int i = 0; i < N_DIRECTIONAL_LIGHTS; ++i) {
        DirectionalLight *light = &DIRECTIONAL_LIGHTS[i];
        set_light_shader_value(shader, *(Light *)light, name, i);
        SetShaderValueV(
            shader,
            GetShaderLocation(shader, TextFormat("directional_lights[%d].direction", i)),
            &light->direction,
            SHADER_UNIFORM_VEC3,
            1
        );
        SetShaderValueMatrix(
            shader,
            GetShaderLocation(shader, TextFormat("directional_lights[%d].vp", i)),
            light->vp
        );

        rlActiveTextureSlot(1 + i);
        rlEnableTexture(light->shadow_map.texture);
        int loc = GetShaderLocation(shader, "directional_lights_shadow_map");
        rlSetUniformSampler(loc + i, light->shadow_map.texture);
    }
    set_n_lights_shader_value(shader, N_DIRECTIONAL_LIGHTS, name);

    // -------------------------------------------------------------------
    // Point lights
    name = "point";
    for (int i = 0; i < N_POINT_LIGHTS; ++i) {
        set_light_shader_value(shader, *(Light *)&POINT_LIGHTS[i], name, i);
        SetShaderValueV(
            shader,
            GetShaderLocation(shader, TextFormat("point_lights[%d].position", i)),
            &POINT_LIGHTS[i].position,
            SHADER_UNIFORM_VEC3,
            1
        );
        SetShaderValueV(
            shader,
            GetShaderLocation(shader, TextFormat("point_lights[%d].attenuation", i)),
            &POINT_LIGHTS[i].attenuation,
            SHADER_UNIFORM_VEC3,
            1
        );
    }
    set_n_lights_shader_value(shader, N_POINT_LIGHTS, name);
}
