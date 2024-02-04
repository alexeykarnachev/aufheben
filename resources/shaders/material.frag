in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;
in vec3 fragPosition;

in vec4 directional_lights_pos[16];


uniform vec4 colDiffuse;
uniform sampler2D texture0;


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
    for (int i = 0; i < n_ambient_lights; ++i) {
        AmbientLight light = ambient_lights[i];
        total_light += light.color * light.intensity;
    }

    // Diffuse
    for (int i = 0; i < n_directional_lights; ++i) {
        float light_depth = texture(directional_lights_shadow_map[i], uv).r;
        vec4 light_pos = directional_lights_pos[i];
        vec3 light_ndc = (0.5 * light_pos.xyz / light_pos.w) + 0.5;
        float depth = texture(directional_lights_shadow_map[i], light_ndc.xy).r;
        float shadow = 1.0;
        if (depth < (light_ndc.z - 0.001)) {
            shadow = 0.0;
        }

        DirectionalLight light = directional_lights[i];
        vec3 direction = normalize(light.direction);
        vec3 light_internal = shadow * get_light_internal(
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

