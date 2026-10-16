#include "Penger.hpp"

namespace Luna {
Penger::Penger() {

}

Penger::~Penger() {

}

void Penger::Draw(IShader &shader) {
    model.Draw(shader);
}
}
