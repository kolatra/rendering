#version 330 core
out vec4 FragColor;
in vec4 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform vec4 greenShade;

uniform float u_time;

void main() {
    // FragColor = ourColor;
    // FragColor = greenShade;
    FragColor = texture(ourTexture, TexCoord);
}