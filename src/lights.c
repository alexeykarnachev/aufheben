#include "lights.h"

#include "raylib.h"

#define MAX_N_AMBIENT_LIGHTS 16
#define MAX_N_DIRECTIONAL_LIGHTS 16
#define MAX_N_POINT_LIGHTS 16

typedef struct Light {
    Color color;
    float intensity;
} Light;

typedef Light AmbientLight;

typedef struct DirectionalLight {
    Light light;
    Vector3 direction;

    unsigned int fbo;
    unsigned int shadowmap;
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

void load_lights(void) {}

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
    light->direction = direction;

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
        set_light_shader_value(shader, *(Light *)&DIRECTIONAL_LIGHTS[i], name, i);
        SetShaderValueV(
            shader,
            GetShaderLocation(shader, TextFormat("directional_lights[%d].direction", i)),
            &DIRECTIONAL_LIGHTS[i].direction,
            SHADER_UNIFORM_VEC3,
            1
        );
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
