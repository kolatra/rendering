#pragma once
#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "graphics/Renderer.hpp"

namespace Luna::Assets {
    namespace Bundle {
        // void SerializeAssets(std::string path);
    }

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
}
