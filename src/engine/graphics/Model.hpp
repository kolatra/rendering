#pragma once

#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "IRenderer.hpp"
#include <iostream>

class Model {
public:
    explicit Model(const std::string &path, bool gamma = false) : gammaCorrection(gamma) {
        loadModel(path);
        std::cout << "Model loaded: " << path << "\n"
        << "    Meshes: " << meshes.size() << "\n"
        << "        Vertices: " << meshes[0].vertices.size() << "\n"
        << "        Indices: " << meshes[0].indices.size() << "\n";
    }
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
