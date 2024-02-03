in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;
in vec3 fragPosition;

struct AmbientLight {
    vec3 color;
    float intensity;
};

struct DirectionalLight {
    vec3 color;
    float intensity;

    vec3 direction;
};

struct PointLight {
    vec3 color;
    float intensity;

    vec3 position;
    vec3 attenuation;
};

uniform vec4 colDiffuse;
uniform sampler2D texture0;

uniform AmbientLight ambient_light;

uniform int n_directional_lights;
uniform DirectionalLight directional_lights[16];

uniform int n_point_lights;
uniform PointLight point_lights[16];

out vec4 finalColor;

// Simple lighting implementation
// https://ogldev.org/www/tutorial20/tutorial20.html

vec3 get_light_internal(vec3 color, vec3 direction, vec3 normal, float intensity) {
    float factor = dot(normal, -direction);

    vec3 diffuse_color = vec3(0.0, 0.0, 0.0);

    if (factor > 0) {
        diffuse_color = color * intensity * factor;
    }

    return diffuse_color;
}

void main() {
    vec2 uv = fragTexCoord;
    vec3 normal = normalize(fragNormal);

    // Base color
    vec3 base_color = texture(texture0, uv).rgb * colDiffuse.rgb;

    // -------------------------------------------------------------------
    // Lighting
    vec3 total_light = vec3(0.0, 0.0, 0.0);

    // Ambient
    total_light += ambient_light.color * ambient_light.intensity;

    // Diffuse
    for (int i = 0; i < n_directional_lights; ++i) {
        DirectionalLight light = directional_lights[i];
        vec3 direction = normalize(light.direction);
        vec3 light_internal = get_light_internal(
            light.color, direction, normal, light.intensity);
        total_light += light_internal;
    }

    for (int i = 0; i < n_point_lights; ++i) {
        PointLight light = point_lights[i];
        vec3 direction = normalize(fragPosition - light.position);
        vec3 light_internal = get_light_internal(
            light.color, direction, normal, light.intensity);
        float dist = distance(light.position, fragPosition);
        float attenuation = 1.0 / dot(light.attenuation, vec3(1.0, dist, dist * dist));
        total_light += light_internal * attenuation;
    }

    // Final color
    vec3 color = total_light * base_color;
    finalColor = vec4(color, 1.0);
}

