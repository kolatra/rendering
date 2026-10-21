#pragma once

#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include <engine/graphics/opengl/RendererOpenGL.hpp>

namespace Luna::OpenGL {
class Model {
public:
    Model() = default;
    explicit Model(const std::string &path, bool gamma = false) : gammaCorrection(gamma) { loadModel(path); }
    ~Model();
    void Draw(IShader &shader);

    // model data
    std::vector<Texture> textures_loaded;
    std::vector<GlMesh> meshes;
    std::string directory;
    bool gammaCorrection;

    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    GlMesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};
}
