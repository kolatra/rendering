#pragma once

#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <iostream>

#include <engine/graphics/opengl/RendererOpenGL.hpp>

namespace Luna::OpenGL {
class Model : public IModel {
public:
    Model() = delete;
    explicit Model(std::string path, bool gamma = false) {
        this->gammaCorrection = gamma;
        Model::Load(path);
    }

    ~Model() override;
    void Draw(IShader &shader) override;

    void Load(std::string &path) override;
    void ProcessNode(aiNode *node, const aiScene *scene) override;
    IMesh* ProcessMesh(aiMesh *mesh, const aiScene *scene) override;
    std::vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName) override;
};
}
