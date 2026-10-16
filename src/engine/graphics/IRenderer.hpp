#pragma once
#include "../platform/IWindow.hpp"

#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace Luna {
class IShader {
public:
    virtual ~IShader() = default;
    virtual void use() = 0;
    virtual void setBool(const std::string &name, bool value) = 0;
    virtual void setInt(const std::string &name, int value) = 0;
    virtual void setFloat(const std::string &name, float value) = 0;
};

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
    uint32_t id;
    std::string type;
    std::string path;
};

class IMesh {
public:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Texture> textures;
    virtual ~IMesh() = default;
    virtual void Draw(IShader &shader) = 0;
};


class IRenderer {
public:
    virtual ~IRenderer() = default;

    virtual void init(IWindow *window, int width, int height) = 0;
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;

    virtual void drawMesh(IMesh& mesh, IShader& shader, glm::mat4 transform) = 0;

    virtual void clear(const glm::vec4& colour) = 0;
};
}
