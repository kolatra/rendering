#ifndef ENGINE_V29090909_IRENDERER_HPP
#define ENGINE_V29090909_IRENDERER_HPP
#include "../platform/IWindow.hpp"

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "IShader.hpp"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    unsigned int id;
    std::string type;
};

class GLShader : public IShader {
public:
    unsigned int ID {};

    GLShader();
    void use() override;
    void setBool(const std::string &name, bool value) override;
    void setInt(const std::string &name, int value) override;
    void setFloat(const std::string &name, float value) override;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    Mesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, std::vector<Texture> &textures);
    void Draw(GLShader &shader);
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

    virtual void drawMesh(const Mesh& mesh, const GLShader& shader, glm::mat4 transform) = 0;

    virtual void clear(const glm::vec4& colour) = 0;
};

#endif //ENGINE_V29090909_IRENDERER_HPP
