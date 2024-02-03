in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

struct AmbientLight {
    vec3 color;
    float intensity;
};

struct DirectionalLight {
    vec3 color;
    float intensity;
    vec3 direction;
};

uniform vec4 colDiffuse;
uniform sampler2D texture0;

uniform AmbientLight ambient_light;

uniform int n_directional_lights;
uniform DirectionalLight directional_lights[4];

out vec4 finalColor;

void main() {
    vec2 uv = fragTexCoord;
    vec3 norm = normalize(fragNormal);

    // Base color
    vec3 base_color = texture(texture0, uv).rgb * colDiffuse.rgb;

    // Ambient
    vec3 ambient = ambient_light.color * ambient_light.intensity * base_color;

    // Diffuse
    vec3 diffuse = vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < n_directional_lights; ++i) {
        DirectionalLight light = directional_lights[i];
        vec3 direction = normalize(light.direction);
        float factor = max(dot(norm, -direction), 0.0);
        diffuse += factor * light.color * light.intensity * base_color;
    }

    // Final color
    vec3 color = ambient + diffuse;
    finalColor = vec4(color, 1.0);
}

