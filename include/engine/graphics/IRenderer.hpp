#pragma once
#include "../platform/IWindow.hpp"

#include <glm/glm.hpp>
#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "engine/core/Camera.hpp"

namespace Luna {
class IShader {
public:
    std::string name = "Unknown";
    virtual ~IShader() = default;
    virtual void use() = 0;
    virtual void setBool(const std::string &name, bool value) = 0;
    virtual void setInt(const std::string &name, int value) = 0;
    virtual void setFloat(const std::string &name, float value) = 0;
    virtual void setMat4(const std::string &name, const glm::mat4 &value) = 0;
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
    virtual void SetupMesh() = 0;
};

class IModel {
public:
    virtual ~IModel() = default;
    std::vector<Texture> textures_loaded;
    std::vector<IMesh*> meshes;
    std::string directory;
    bool gammaCorrection;

    void Load(std::string &path);
    void ProcessNode(aiNode *node, const aiScene *scene);
    IMesh* ProcessMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
    void Draw(IShader &shader) const;
};

class IRenderer {
public:
    std::vector<IShader*> shaders;
    std::unique_ptr<EngineCamera> camera;
    virtual ~IRenderer() = default;

    virtual void init(IWindow *window, Configuration *config) = 0;
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;

    virtual void clear(const glm::vec4& colour) = 0;
    virtual void setCulling(bool flag) = 0;
};
}
