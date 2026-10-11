#include "Penger.hpp"

Penger::Penger() {

}

Penger::~Penger() {

}

void Penger::Draw(IShader &shader) {
    model.Draw(shader);
}
