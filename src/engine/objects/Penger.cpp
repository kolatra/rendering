#include "Penger.hpp"

namespace Luna {
Penger::Penger() : model("penger-obj-main/penger/penger.obj", false) {
}

Penger::~Penger() {

}

void Penger::Draw(IShader &shader) {
    model.Draw(shader);
}
}
