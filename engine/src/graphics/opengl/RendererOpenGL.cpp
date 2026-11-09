#include "RendererOpenGL.hpp"
#include "../../core/Config.hpp"
#include "../../Utilities.hpp"
#include <iostream>
#include <spdlog/spdlog.h>
#include <glm/gtc/type_ptr.hpp>
#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Luna::OpenGL {
    void Implementation::init(IWindow *window, Configuration *config) {
        // glfw has already initialized OpenGL in the background, so here we can
        // set our application specific settings
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_PROGRAM_POINT_SIZE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glViewport(0, 0, config->width, config->height);
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

    void Implementation::beginFrame() {
        this->clear({0.2f, 0.3f, 0.3f, 1.0f});
    }

    Implementation::~Implementation() {
        // TODO: i still need to cleanup other resources here
        for (const auto *shader : shaders) {
            delete shader;
        }

        for (const auto *object : objects) {
            delete object;
        }
    }

    void Implementation::clear(const glm::vec4 &colour) {
        glClearColor(colour.r, colour.g, colour.b, colour.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    GLShader::GLShader(const std::string& name) {
        auto vertex_path = "shaders/" + name + ".vsh";
        auto fragment_path = "shaders/" + name + ".fsh";
        const auto vCode = Util::ReadFileToString(vertex_path);
        const auto fCode = Util::ReadFileToString(fragment_path);
        const char* vertexCode = vCode.c_str();
        const char* fragmentCode = fCode.c_str();

        const uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexCode, nullptr);
        glCompileShader(vertexShader);
        GLint success = GL_FALSE;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (success != GL_TRUE) {
            GLint logLength = 0;
            glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logLength);
            std::string log(static_cast<size_t>(logLength), '\0');
            glGetShaderInfoLog(vertexShader, logLength, nullptr, log.data());
            spdlog::error("Vertex shader compilation failed ({}):\n{}", vertex_path, log);
        }

        const uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentCode, nullptr);
        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (success != GL_TRUE) {
            GLint logLength = 0;
            glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &logLength);
            std::string log(static_cast<size_t>(logLength), '\0');
            glGetShaderInfoLog(fragmentShader, logLength, nullptr, log.data());
            spdlog::error("Fragment shader compilation failed ({}):\n{}", fragment_path, log);
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
            spdlog::error("Shader program link failed:\n{}", log);
            exit(1);
        }

        spdlog::debug("Shader {} created from {}", ID, vertex_path);
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

            spdlog::debug("Loaded texture from: {}", path);
            stbi_image_free(data);
        } else {
            spdlog::error("Texture failed to load from: {}", path);
            stbi_image_free(data);
        }

        return textureID;
    }
}
