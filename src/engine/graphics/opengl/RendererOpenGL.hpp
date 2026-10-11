#pragma once
#include "../IRenderer.hpp"

class RendererOpenGL : public IRenderer {
public:
    void init(IWindow *window, int width, int height) override;
    void beginFrame() override;
    void endFrame() override;
    void drawMesh(const Mesh &mesh, const GLShader &shader, glm::mat4 transform) override;
    void clear(const glm::vec4 &colour) override;
};

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);
