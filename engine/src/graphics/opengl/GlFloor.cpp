#include "graphics/opengl/RendererOpenGL.hpp"
#include "glad/gl.h"

namespace Luna::OpenGL {
    GlFloor::GlFloor() {
        this->texture = Texture(TextureFromFile("floor.png", "assets"));
        this->shader = new GLShader("floor");
    }

    GlFloor::~GlFloor() {
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

    void GlFloor::Setup() {
        constexpr auto SquareCount = 9;
        std::vector<Vertex> vertices(SquareCount * SquareCount * 4);
        std::vector<uint32_t> indices(SquareCount * SquareCount * 6);
        for (uint32_t x = 0; x < SquareCount; x++) {
            for (uint32_t z = 0; z < SquareCount; z++) {
                const auto base = static_cast<uint32_t>(vertices.size());

                vertices.insert(vertices.end(), {
                    {{x + 0.5f, 0.0f, z - 0.5f}, {1, 0, 0}, {1, 1}},
                    {{x + 0.5f, 0.0f, z + 0.5f}, {0, 1, 0}, {1, 0}},
                    {{x - 0.5f, 0.0f, z + 0.5f}, {0, 0, 1}, {0, 0}},
                    {{x - 0.5f, 0.0f, z - 0.5f}, {1, 1, 0}, {0, 1}},
                });

                indices.insert(indices.end(), {
                    base, base + 1, base + 3,
                    base + 1, base + 2, base + 3
                });
            }
        }

        indexCount = static_cast<uint32_t>(indices.size());;

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Position)));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, Normal)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, TexCoords)));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void GlFloor::Render(EngineCamera &camera) {
        shader->use();

        shader->setMat4("view", camera.view);
        shader->setMat4("projection", camera.projection);

        glBindTexture(GL_TEXTURE_2D, texture.id);
        glBindVertexArray(VAO);
        shader->setMat4("model", glm::mat4(1.0f));
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}