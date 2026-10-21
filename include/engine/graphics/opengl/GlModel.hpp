#pragma once

#include <string>
#include <iostream>
#include <engine/graphics/opengl/RendererOpenGL.hpp>

namespace Luna::OpenGL {
class GlModel : public IModel {
public:
    GlModel() = delete;
    explicit GlModel(std::string path, bool gamma = false) {
        this->gammaCorrection = gamma;
        Load(path);
    }

    ~GlModel() override;
};
}
