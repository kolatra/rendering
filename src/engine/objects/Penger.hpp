#pragma once
#include "engine/graphics/Model.hpp"

namespace Luna {
class Penger {
public:
    OpenGL::Model model;
    Penger();
    ~Penger();
    void Draw(IShader &shader);
};
}
