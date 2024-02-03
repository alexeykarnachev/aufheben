in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec4 u_border_color;

out vec4 finalColor;

void main() {
    vec2 uv = fragTexCoord;
    vec4 tex_color = texture(texture0, uv);
    finalColor = tex_color * colDiffuse; 
}

