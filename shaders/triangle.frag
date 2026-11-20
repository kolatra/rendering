#version 460
layout(location = 0) out vec4 FragColor;
layout(location = 0) in vec2 TexCoord;

#ifdef VULKAN
layout(set = 0, binding = 0) uniform sampler2D texture_diffuse1;
#else
layout(binding = 0) uniform sampler2D texture_diffuse1;
#endif

void main() {
    FragColor = vec4(texture(texture_diffuse1, TexCoord).rgb, 1.0);
    // For UV visualization:
    // FragColor = vec4(TexCoord, 0.0, 1.0);
}
