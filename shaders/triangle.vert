#version 460
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;

layout (location = 0) out vec2 TexCoord;

#ifdef VULKAN
layout (set = 0, binding = 1) uniform Transform {
    mat4 model;
    mat4 view;
    mat4 projection;
} transform;
#else
layout (location = 0) uniform mat4 model;
layout (location = 4) uniform mat4 view;
layout (location = 8) uniform mat4 projection;
#endif

void main() {
#ifdef VULKAN
    gl_Position = transform.projection * transform.view * transform.model * vec4(aPos, 1.0);
#else
    gl_Position = projection * view * model * vec4(aPos, 1.0);
#endif
    TexCoord = aTexCoord;
}
