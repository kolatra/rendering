#pragma once
#include "../platform/IWindow.hpp"

#include <glm/glm.hpp>
#include <string>
#include <vector>

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
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    Mesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, std::vector<Texture> &textures);
    void Draw(IShader &shader);
private:
    unsigned int VAO, VBO, EBO;
    void setupMesh();
};

class IRenderer {
public:
    virtual ~IRenderer() = default;

    virtual void init(IWindow *window, int width, int height) = 0;
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;

    virtual void drawMesh(const Mesh& mesh, IShader& shader, glm::mat4 transform) = 0;

    virtual void clear(const glm::vec4& colour) = 0;
};
