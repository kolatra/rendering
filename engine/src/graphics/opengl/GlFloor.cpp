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
        // Set of vertices for a single square
        // TODO: use a loop to add this constexpr to a vector of vertices and indices, offset somehow idk yet
        constexpr float vertices[] = {
            // positions          // colors           // texture coords
            0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
            0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
           -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
           -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
        };

        constexpr uint32_t indices[] = {
            0, 1, 3,
            1, 2, 3
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(6 * sizeof(float)));
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
        constexpr auto SquareCount = 9;

        // TODO: Could this be optimized into a single draw call?
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        for (uint32_t x = 0; x < SquareCount; x++) {
            for (uint32_t z = 0; z < SquareCount; z++) {
                auto model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(x, 0, z));
                model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                shader->setMat4("model", model);

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
            }
        }
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}