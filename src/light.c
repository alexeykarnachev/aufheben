#include "light.h"

#include "raylib.h"

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

void set_ambient_lights_shader_value(Shader shader, AmbientLight *lights, int n) {
    const char *name = "ambient";
    for (int i = 0; i < n; ++i) {
        set_light_shader_value(shader, lights[i], name, i);
    }
    set_n_lights_shader_value(shader, n, name);
}

void set_directional_lights_shader_value(Shader shader, DirectionalLight *lights, int n) {
    const char *name = "directional";
    for (int i = 0; i < n; ++i) {
        set_light_shader_value(shader, *(Light *)&lights[i], name, i);
        SetShaderValueV(
            shader,
            GetShaderLocation(shader, TextFormat("directional_lights[%d].direction", i)),
            &lights[i].direction,
            SHADER_UNIFORM_VEC3,
            1
        );
    }
    set_n_lights_shader_value(shader, n, name);
}

void set_point_lights_shader_value(Shader shader, PointLight *lights, int n) {
    const char *name = "point";
    for (int i = 0; i < n; ++i) {
        set_light_shader_value(shader, *(Light *)&lights[i], name, i);
        SetShaderValueV(
            shader,
            GetShaderLocation(shader, TextFormat("point_lights[%d].position", i)),
            &lights[i].position,
            SHADER_UNIFORM_VEC3,
            1
        );
        SetShaderValueV(
            shader,
            GetShaderLocation(shader, TextFormat("point_lights[%d].attenuation", i)),
            &lights[i].attenuation,
            SHADER_UNIFORM_VEC3,
            1
        );
    }
    set_n_lights_shader_value(shader, n, name);
}
