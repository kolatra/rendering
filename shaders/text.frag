#version 460
layout (location = 0) in vec2 TexCoords;
layout (location = 0) out vec4 color;

#ifdef VULKAN
#else
layout (location = 0) uniform sampler2D text;
layout (location = 1) uniform vec3 textColor;
#endif

void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    color = vec4(textColor, 1.0) * sampled;
}