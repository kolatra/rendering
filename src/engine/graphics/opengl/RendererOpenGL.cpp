#include "engine/graphics/opengl/RendererOpenGL.hpp"

#include <iostream>
#include <print>

#include "engine/core/Application.hpp"
#include "glad/gl.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <sstream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>

namespace Luna::OpenGL {
    Implementation::Implementation() {
        auto *shader = new GLShader("shaders/triangle.vsh", "shaders/triangle.fsh");
        shader->name = "Triangles";
        this->shaders.push_back(shader);
    }

    void Implementation::init(IWindow *window, Configuration *config) {
        // glfw has already initialized OpenGL in the background, so here we can
        // set our application specific settings
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        // The Penger OBJ includes an inverted hull for its outline. Cull its
        // back faces so the hull does not cover the textured body.
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        glEnable(GL_PROGRAM_POINT_SIZE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glViewport(0, 0, config->width, config->height);
    }

    void Implementation::beginFrame() {

    }

    void Implementation::endFrame() {

    }

    Implementation::~Implementation() {
        // TODO: i still need to cleanup other resources here
        for (const auto *shader : shaders) {
            delete shader;
        }
    }

    void Implementation::clear(const glm::vec4 &colour) {
        glClearColor(colour.r, colour.g, colour.b, colour.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    GLShader::GLShader(const std::string& vertex_path, const std::string& fragment_path) {
        std::string vCode, fCode;
        std::ifstream vertexFile, fragmentFile;
        vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fragmentFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            vertexFile.open(vertex_path);
            fragmentFile.open(fragment_path);
            std::stringstream vertexStream, fragmentStream;
            vertexStream << vertexFile.rdbuf();
            fragmentStream << fragmentFile.rdbuf();
            vertexFile.close();
            fragmentFile.close();
            vCode = vertexStream.str();
            fCode = fragmentStream.str();
        } catch (const std::ifstream::failure &) {
            std::cerr << "Could not open shader files: " << vertex_path
                      << " and " << fragment_path << '\n';
        }

        const char* vertexCode = vCode.c_str();
        const char* fragmentCode = fCode.c_str();

        uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexCode, NULL);
        glCompileShader(vertexShader);
        GLint success = GL_FALSE;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (success != GL_TRUE) {
            GLint logLength = 0;
            glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logLength);
            std::string log(static_cast<size_t>(logLength), '\0');
            glGetShaderInfoLog(vertexShader, logLength, nullptr, log.data());
            std::cerr << "Vertex shader compilation failed (" << vertex_path << "):\n" << log << '\n';
        }

        uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentCode, NULL);
        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (success != GL_TRUE) {
            GLint logLength = 0;
            glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &logLength);
            std::string log(static_cast<size_t>(logLength), '\0');
            glGetShaderInfoLog(fragmentShader, logLength, nullptr, log.data());
            std::cerr << "Fragment shader compilation failed (" << fragment_path << "):\n" << log << '\n';
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
            std::cerr << "Shader program link failed:\n" << log << '\n';
            exit(1);
        }

        std::cout << "Shader " << ID << " created.\n";
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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

            std::cout << "Loaded texture from: " << path << std::endl;
            stbi_image_free(data);
        } else {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }

    Floor::~Floor() {
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

    void Floor::Setup() {
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

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void Floor::Render() {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture.id);

        shader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }
}
