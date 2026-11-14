#include "LunaEngine.hpp"
#include "spdlog/spdlog.h"

namespace {

struct BusinessMan : Luna::GameObject {
    explicit BusinessMan() {
        modelPath = "assets/penger-obj-main/suitger/suitedpenger.obj";
        culling = true;
        position = glm::translate(position, glm::vec3(4.0f, 0.0f, 4.0f));
    }

    void Tick() override {
        position = glm::rotate(position, glm::radians(-0.1f), glm::vec3(0.0f, 0.0f, 1.0f));
    }
};

}

int main(int argc, char **argv) {
    const auto app = Luna::Initialize(argc, argv);

    auto *ourScene = new Luna::Scene(app, /*controls = */true);
    ourScene->RegisterObject<BusinessMan>();
    app->renderer->LoadScene(ourScene);

    return app->Run();
}
