in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

struct AmbientLight {
    vec3 color;
    float intensity;
};

uniform vec4 colDiffuse;
uniform sampler2D texture0;
uniform AmbientLight ambient_light;

out vec4 finalColor;

void main() {
    vec2 uv = fragTexCoord;

    // Base color
    vec3 base_color = texture(texture0, uv).rgb * colDiffuse.rgb;

    // Ambient
    vec3 ambient = ambient_light.color * ambient_light.intensity * base_color;

    // Diffuse
    // vec3 norm = normalize(fragNormal);

    // Final color
    vec3 color = ambient;
    finalColor = vec4(color, 1.0);
}

