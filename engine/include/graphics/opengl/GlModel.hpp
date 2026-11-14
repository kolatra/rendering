#pragma once

#include <string>
#include "RendererOpenGL.hpp"

namespace Luna::OpenGL {
class GlModel : public Model {
public:
    GlModel() = delete;
    explicit GlModel(std::string path, bool gamma = false) {
        this->gammaCorrection = gamma;
        Load(path);
    };

    ~GlModel() override;
};
}
