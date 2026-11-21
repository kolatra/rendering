#include <ranges>
#include <stdexcept>

#include <freetype/freetype.h>
#include <fmt/core.h>
#include "glad/gl.h"
#include "glm/gtc/type_ptr.hpp"

#include "graphics/Renderer.hpp"
#include "graphics/Text.hpp"
#include "graphics/types/Shader.hpp"

namespace Luna::FontRenderer::OpenGLText {
    static std::unique_ptr<Shader> shader;
    static uint32_t c_VAO = 0;
    static uint32_t c_VBO = 0;

    void init(void* f) {
        auto face = static_cast<FT_Face>(f);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        for (uint8_t c = 0; c < 128; c++) {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
                throw std::runtime_error("Could not render character");
            }

            uint32_t texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            Character character {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<uint32_t>(face->glyph->advance.x)
            };
            AddCharacter(c, character);
        }

        shader = Shader::Create("text");
        fmt::println("Text data shader created");

        glGenVertexArrays(1, &c_VAO);
        glGenBuffers(1, &c_VBO);
        glBindVertexArray(c_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, c_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        fmt::println("Text data buffered");
    }

    void shutdown() {
        for (const auto &glyph: character_map | std::views::values) {
            if (glyph.texture_id) {
                glDeleteTextures(1, &glyph.texture_id);
            }
        }
        if (c_VAO) {
            glDeleteVertexArrays(1, &c_VAO);
            c_VAO = 0;
        }
        if (c_VBO) {
            glDeleteBuffers(1, &c_VBO);
            c_VBO = 0;
        }
        shader.reset();
    }

    void render(const std::string &text, float x, float y, float scale, glm::vec3 colour) {
        GLint polygonMode[2];
        glGetIntegerv(GL_POLYGON_MODE, polygonMode);
        const GLint oldMode = polygonMode[0];
        // Render as FILL regardless of previous mode, sets back after
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        shader->use();
        glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(1280), 0.0f, static_cast<float>(720));
        glUniformMatrix4fv(glGetUniformLocation(shader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3f(glGetUniformLocation(shader->ID, "textColor"), colour.x, colour.y, colour.z);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(c_VAO);

        for (auto c = text.begin(); c != text.end(); c++) {
            auto [textureID, size, bearing, advance] = character_map[*c];

            float xpos = x + bearing.x * scale;
            float ypos = y - (size.y - bearing.y) * scale;
            float w = size.x * scale;
            float h = size.y * scale;
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };
            // Render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, textureID);
            // update VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, c_VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            x += (advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glPolygonMode(GL_FRONT_AND_BACK, oldMode);
    }
}
