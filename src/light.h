#pragma once

#include "raylib.h"

// https://learnopengl.com/Lighting/Light-casters
#define ATTENUATION_7 ((Vector3){1.0, 0.70, 1.80})
#define ATTENUATION_13 ((Vector3){1.0, 0.35, 0.44})
#define ATTENUATION_20 ((Vector3){1.0, 0.22, 0.20})
#define ATTENUATION_32 ((Vector3){1.0, 0.14, 0.07})
#define ATTENUATION_50 ((Vector3){1.0, 0.09, 0.032})
#define ATTENUATION_65 ((Vector3){1.0, 0.07, 0.017})
#define ATTENUATION_100 ((Vector3){1.0, 0.045, 0.0075})
#define ATTENUATION_160 ((Vector3){1.0, 0.027, 0.0028})
#define ATTENUATION_200 ((Vector3){1.0, 0.022, 0.0019})
#define ATTENUATION_325 ((Vector3){1.0, 0.014, 0.0007})
#define ATTENUATION_600 ((Vector3){1.0, 0.007, 0.0002})
#define ATTENUATION_3250 ((Vector3){1.0, 0.0014, 0.000007})

typedef struct Light {
    Color color;
    float intensity;
} Light;

typedef Light AmbientLight;

typedef struct DirectionalLight {
    Light light;
    Vector3 direction;
} DirectionalLight;

typedef struct PointLight {
    Light light;
    Vector3 position;
    Vector3 attenuation;
} PointLight;

void set_ambient_lights_shader_value(Shader shader, AmbientLight *lights, int n);
void set_directional_lights_shader_value(Shader shader, DirectionalLight *lights, int n);
void set_point_lights_shader_value(Shader shader, PointLight *lights, int n);
