#include "RendererOpenGL.hpp"

#include <iostream>
#include <print>

#include "../../Application.hpp"
#include "glad/gl.h"
#include "../../platform/glfw/WindowGLFW.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <sstream>
#include <fstream>

void RendererOpenGL::init(IWindow *window, int width, int height) {
    // glfw has already initialized OpenGL in the background, so here we can
    // set our application specific settings
    glEnable(GL_BLEND);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glViewport(0, 0, width, height);
}

void RendererOpenGL::beginFrame() {

}

void RendererOpenGL::endFrame() {

}

void RendererOpenGL::drawMesh(const GlMesh &mesh, IShader &shader, glm::mat4 transform) {
    auto gl_shader = static_cast<GLShader&>(shader);
}

void RendererOpenGL::clear(const glm::vec4 &colour) {
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
    // sanityCheck(vertexShader);

    uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentCode, NULL);
    glCompileShader(fragmentShader);
    // sanityCheck(fragmentShader);

    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);
    // programSanityCheck(program_id);

    // delete shader objects from RAM, as they're on the GPU
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
    // glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int>(value));
}

void GLShader::setFloat(const std::string &name, float value) {
    // glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void GLShader::setInt(const std::string &name, int value) {
    // glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

GlMesh::GlMesh(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices, std::vector<Texture> &textures) {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

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

        shader.setInt(("material." + name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

uint32_t TextureFromFile(const char *path, const std::string &directory, bool gamma) {
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    uint32_t textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format = 0;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
