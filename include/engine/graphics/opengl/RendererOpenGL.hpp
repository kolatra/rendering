#pragma once
#include <engine/graphics/IRenderer.hpp>

namespace Luna::OpenGL {
    class GlMesh : public IMesh {
public:
    GlMesh(const GlMesh&) = delete;
    GlMesh& operator=(const GlMesh&) = delete;

    GlMesh(GlMesh &&other) noexcept :
    VAO(std::exchange(other.VAO, 0)),
    VBO(std::exchange(other.VBO, 0)),
    EBO(std::exchange(other.EBO, 0))
    {
        vertices = std::move(other.vertices);
        indices = std::move(other.indices);
        textures = std::move(other.textures);
    }

    GlMesh &operator=(GlMesh &&other) noexcept {
        if (this == &other) {
            return *this;
        }

        Release();

        this->VAO = std::exchange(other.VAO, 0);
        this->VBO = std::exchange(other.VBO, 0);
        this->EBO = std::exchange(other.EBO, 0);
        this->vertices = std::move(other.vertices);
        this->indices = std::move(other.indices);
        this->textures = std::move(other.textures);
        return *this;
    }

    GlMesh(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices, std::vector<Texture> &textures);
    ~GlMesh() override;
    void Draw(IShader &shader) override;
    void SetupMesh() override;
private:
    void Release() noexcept;

    uint32_t VAO {}, VBO {}, EBO {};
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
    void setMat4(const std::string &name, const glm::mat4 &value) override;
};

class Implementation : public IRenderer {
public:
    Implementation(Configuration *config);
    void init(IWindow *window, Configuration *config) override;
    ~Implementation() override;
    void beginFrame() override;
    void endFrame() override;
    void clear(const glm::vec4 &colour) override;

    void setCulling(bool flag) override;
};

uint32_t TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

struct Floor {
    Texture texture;
    GLShader shader;
    uint32_t VAO{}, VBO{}, EBO{};

    Floor() :
    texture(TextureFromFile("floor.png", "assets")),
    shader("shaders/floor.vsh", "shaders/floor.fsh"){}
    ~Floor();

    void Setup();

    void Render(EngineCamera &camera);
};
}
