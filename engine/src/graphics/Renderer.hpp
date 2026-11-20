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

#include "platform/Window.hpp"
#include "core/Camera.hpp"

namespace Luna {
class AppCore;
struct Configuration;
class Renderer;
class Scene;
class GameObject;
class Shader;

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
    static std::unique_ptr<Model> Create(std::string path, bool gamma = false);
    virtual ~Model();
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
    float deltaTime;
    Renderer();
    virtual ~Renderer();

    virtual void init(Window *window) = 0;
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;

    virtual void clear(const glm::vec4& colour) = 0;
    virtual void setCulling(bool flag) = 0;
};

struct Floor {
    Texture texture;
    std::shared_ptr<Shader> shader;

    static std::unique_ptr<Floor> Create();
    virtual ~Floor() = default;
    virtual void Setup() = 0;
    virtual void Render(EngineCamera &camera) = 0;
};

std::unique_ptr<Renderer> StartGraphics();
}
