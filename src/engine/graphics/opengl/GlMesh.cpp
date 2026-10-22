#include <engine/graphics/opengl/RendererOpenGL.hpp>

#include "glad/gl.h"

namespace Luna::OpenGL {
    GlMesh::GlMesh(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices, std::vector<Texture> &textures) {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        GlMesh::SetupMesh();
    }

    void GlMesh::SetupMesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, Normal)));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, TexCoords)));

        glBindVertexArray(0);
    }

    GlMesh::~GlMesh() {
        Release();
    }

    void GlMesh::Release() noexcept {
        if (VAO) {
            glDeleteVertexArrays(1, &VAO);
            VAO = 0;
        }

        if (VBO) {
            glDeleteBuffers(1, &VBO);
            VBO = 0;
        }

        if (EBO) {
            glDeleteBuffers(1, &EBO);
            EBO = 0;
        }
    }

    void GlMesh::Draw(IShader &shader) {
        uint32_t diffuseNr = 1;
        uint32_t specularNr = 1;
        for (uint32_t i = 0; i < textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            std::string number;
            std::string name = textures[i].type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++);

            shader.setInt((name + number).c_str(), static_cast<int>(i));
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        glActiveTexture(GL_TEXTURE0);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
    }
}
