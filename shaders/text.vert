#version 460
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
layout (location = 0) out vec2 TexCoords;

#ifdef VULKAN
layout (set = 0, binding = 1) uniform Transform {
    mat4 projection;
} transform;
#else
layout (location = 4) uniform mat4 projection;
#endif

void main() {
#ifdef VULKAN
    gl_Position = transform.projection * vec4(vertex.xy, 0.0, 1.0);
#else
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
#endif
    TexCoords = vertex.zw;
}
