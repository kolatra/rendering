#include "LunaEngine.hpp"
#include "spdlog/spdlog.h"

namespace {

struct BusinessMan : Luna::GameObject {
    explicit BusinessMan() {
        modelPath = "assets/penger-obj-main/suitger/suitedpenger.obj";
        culling = true;
        position = glm::mat4(1.0f);
        position = glm::translate(position, glm::vec3(4.0f, 0.0f, 4.0f));
    }

    void Tick() override {
        position = glm::rotate(position, glm::radians(-0.1f), glm::vec3(1.0f, 0.0f, 0.0f));
    }
};

}

int main(int argc, char **argv) {
    const auto app = Luna::Initialize(argc, argv);

    app->RegisterObject<BusinessMan>();

    return app->Run();
}
