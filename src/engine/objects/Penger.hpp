#pragma once
#include "engine/graphics/Model.hpp"

namespace Luna {
class Penger {
public:
    Model model { "penger-obj-main/penger/penger.obj", false };
    Penger();
    ~Penger();
    void Draw(IShader &shader);
};
}
