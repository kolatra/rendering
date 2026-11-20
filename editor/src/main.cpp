#include "LunaEngine.hpp"

int main(int argc, char **argv) {
    struct BusinessMan : Luna::GameObject {
        explicit BusinessMan() {
            modelPath = "assets/penger-obj-main/suitger/suitedpenger.obj";
            culling = true;
            // position = glm::translate(position, glm::vec3(0.0f, 0.0f, 0.0f));
        }

        void Tick() override {
            position = glm::rotate(position, glm::radians(-0.01f), glm::vec3(0.0f, 1.0f, 0.0f));
        }

        void PreRender(float dt) override {
            Luna::RenderText("this text is rendered from business man right before rendering him", 25.0f, 25.0f, 0.3f, glm::vec3(1.0f, 0.0f, 0.0f));
        }
    };

    auto app = Luna::Initialize(argc, argv);
    app->currentScene->RegisterObject<BusinessMan>();

    return app->Run();
}
