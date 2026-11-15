#include "LunaEngine.hpp"

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

    void PreRender(float dt) override {
        Luna::RenderText("this text is rendered from business man right before rendering him", 25.0f, 25.0f, 0.3f, glm::vec3(0.0f, 1.0f, 0.0f));
    }
};

}

int main(int argc, char **argv) {
    const auto app = Luna::Initialize(argc, argv);

    auto ourScene = std::make_unique<Luna::Scene>(app, /*controls = */true);
    ourScene->RegisterObject<BusinessMan>();
    app->LoadScene(std::move(ourScene));

    return app->Run();
}
