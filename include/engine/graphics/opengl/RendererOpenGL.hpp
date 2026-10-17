#pragma once
#include "../IRenderer.hpp"

namespace Luna {
class GlMesh : public IMesh {
public:
    GlMesh(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices, std::vector<Texture> &textures);
    ~GlMesh() override;
    void Draw(IShader &shader) override;
private:
    uint32_t VAO, VBO, EBO; // OpenGL specific stuff, needs to be made into a subclass?
};

class GLShader : public IShader {
public:
    uint32_t ID {};

    GLShader(const std::string& vertex_path, const std::string& fragment_path);
    ~GLShader() override;
    void use() override;
    void setBool(const std::string &name, bool value) override;
    void setInt(const std::string &name, int value) override;
    void setFloat(const std::string &name, float value) override;
};

class RendererOpenGL : public IRenderer {
public:
    void init(IWindow *window, int width, int height) override;
    void beginFrame() override;
    void endFrame() override;
    void drawMesh(IMesh &mesh, IShader &shader, glm::mat4 transform) override;
    void clear(const glm::vec4 &colour) override;
};

uint32_t TextureFromFile(const char *path, const std::string &directory, bool gamma = false);
}
