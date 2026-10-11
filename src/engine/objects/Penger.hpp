#pragma once
#include "../graphics/Model.hpp"

class Penger {
public:
    Model model { "penger-obj-main/penger/penger.obj", false };
    Penger();
    ~Penger();
    void Draw(IShader &shader);
};
