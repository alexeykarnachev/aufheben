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

void load_lights(int shadow_map_size);

void clear_ambient_lights(void);
void clear_directional_lights(void);
void clear_point_lights(void);
void clear_lights(void);

bool add_ambient_light(Color color, float intensity);
bool add_directional_light(Color color, float intensity, Vector3 direction);
bool add_point_light(Color color, float intensity, Vector3 position, Vector3 attenuation);

void draw_shadow_maps(void (*draw_scene)());
void set_lights_shader_values(Shader shader);
