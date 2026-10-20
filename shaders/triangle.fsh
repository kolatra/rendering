#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D texture_diffuse1;

uniform float u_time;

void main() {
    FragColor = vec4(texture(texture_diffuse1, TexCoord).rgb, 1.0);
    // For UV visualization:
    // FragColor = vec4(TexCoord, 0.0, 1.0);
}