#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/gtc/matrix_transform.hpp>

#include "platform/IWindow.hpp"
#include "core/Camera.hpp"

namespace Luna {
class AppCore;
struct Configuration;
class Renderer;
class GameObject;

class Shader {
public:
    std::string name = "Unknown";

    [[maybe_unused]] static Shader* Create(const AppCore &app, const std::string &name);
    virtual ~Shader() = default;
    virtual void use() = 0;
    virtual void setBool(const std::string &name, bool value) = 0;
    virtual void setInt(const std::string &name, int value) = 0;
    virtual void setFloat(const std::string &name, float value) = 0;
    virtual void setMat4(const std::string &name, const glm::mat4 &value) = 0;
};

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

struct Texture {
    uint32_t id;
    std::string type;
    std::string path;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Texture> textures;
    virtual ~Mesh() = default;
    virtual void Draw(Shader &shader) = 0;
    virtual void SetupMesh() = 0;
};

class Model {
public:
    static std::shared_ptr<Model> Create(const std::shared_ptr<Configuration> &config, std::string path, bool gamma = false);
    virtual ~Model() = default;
    std::vector<Texture> textures_loaded;
    std::vector<Mesh*> meshes;
    std::string directory;
    bool gammaCorrection;

    void Load(std::string &path);
    void ProcessNode(aiNode *node, const aiScene *scene);
    Mesh* ProcessMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
    void Draw(Shader &shader) const;
};

class Renderer {
public:
    std::vector<Shader*> shaders;
    std::unique_ptr<EngineCamera> camera;
    std::vector<std::shared_ptr<GameObject>> objects;
    float deltaTime;
    virtual ~Renderer() = default;

    virtual void init(IWindow &window) = 0;
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;

    virtual void clear(const glm::vec4& colour) = 0;
    virtual void setCulling(bool flag) = 0;

};

struct Floor {
    Texture texture;
    Shader* shader;

    static std::unique_ptr<Floor> Create(Configuration &config);
    virtual ~Floor() = default;
    virtual void Setup() = 0;
    virtual void Render(EngineCamera &camera) = 0;
};

std::unique_ptr<Renderer> StartBackend(Configuration &config);
}
