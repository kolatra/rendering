#include <iostream>

#include <glm/gtc/type_ptr.hpp>
#include <fmt/core.h>
#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "graphics/impl/opengl/RendererOpenGL.hpp"
#include "core/Config.hpp"
#include "Utilities.hpp"
#include "graphics/scene/Scene.hpp"

namespace Luna::OpenGL {
    void Implementation::init(Window *window) {
        // glfw has already initialized OpenGL in the background, so here we can
        // set our application specific settings
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_PROGRAM_POINT_SIZE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        const auto [width, height] = window->GetFrameBufferSize();
        glViewport(0, 0, width, height);
    }

    void Implementation::setCulling(bool flag) {
        if (flag) {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CCW);
        } else {
            glDisable(GL_CULL_FACE);
        }
    }

    Implementation::~Implementation() {

    }

    void Implementation::clear(const glm::vec4 &colour) {
        glClearColor(colour.r, colour.g, colour.b, colour.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    GLShader::GLShader(const shaderc::CompilationResult<uint32_t> &vCode, const shaderc::CompilationResult<uint32_t> &fCode) {
        std::vector vertexCode(vCode.begin(), vCode.end());
        std::vector fragmentCode(fCode.begin(), fCode.end());
        const uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderBinary(1, &vertexShader, GL_SHADER_BINARY_FORMAT_SPIR_V, vertexCode.data(), vertexCode.size() * sizeof(uint32_t));
        glSpecializeShader(vertexShader, "main", 0, nullptr, nullptr);
        GLint success = GL_FALSE;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (success != GL_TRUE) {
            GLint logLength = 0;
            glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logLength);
            std::string log(static_cast<size_t>(logLength), '\0');
            glGetShaderInfoLog(vertexShader, logLength, nullptr, log.data());
            fmt::println("Vertex shader compilation failed:\n{}", log);
            exit(1);
        }

        const uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderBinary(1, &fragmentShader, GL_SHADER_BINARY_FORMAT_SPIR_V, fragmentCode.data(), fragmentCode.size() * sizeof(uint32_t));
        glSpecializeShader(fragmentShader, "main", 0, nullptr, nullptr);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (success != GL_TRUE) {
            GLint logLength = 0;
            glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &logLength);
            std::string log(static_cast<size_t>(logLength), '\0');
            glGetShaderInfoLog(fragmentShader, logLength, nullptr, log.data());
            fmt::println("Fragment shader compilation failed:\n{}", log);
            exit(1);
        }

        ID = glCreateProgram();
        glAttachShader(ID, vertexShader);
        glAttachShader(ID, fragmentShader);
        glLinkProgram(ID);
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (success != GL_TRUE) {
            GLint logLength = 0;
            glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &logLength);
            std::string log(static_cast<size_t>(logLength), '\0');
            glGetProgramInfoLog(ID, logLength, nullptr, log.data());
            fmt::println("Shader program link failed:\n{}", log);
            exit(1);
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    GLShader::~GLShader() {
        glDeleteProgram(ID);
    }

    void GLShader::use() {
        glUseProgram(ID);
    }

    void GLShader::setBool(const std::string &name, bool value) {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int>(value));
    }

    void GLShader::setFloat(const std::string &name, float value) {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void GLShader::setMat4(const std::string &name, const glm::mat4 &value) {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }

    void GLShader::setInt(const std::string &name, int value) {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    uint32_t TextureFromFile(const char *path, const std::string &directory, bool gamma) {
        std::string filename = std::string(path);
        filename = directory + '/' + filename;

        uint32_t textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nrComponents;
        unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        if (data) {
            GLenum format = 0;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            fmt::println("Loaded texture from: {}", filename);
            stbi_image_free(data);
        } else {
            fmt::println("Texture failed to load from: {}", filename);
            stbi_image_free(data);
        }

        return textureID;
    }
}
