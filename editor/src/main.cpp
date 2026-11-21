#include "LunaEngine.hpp"

int main(int argc, char **argv) {
    using namespace Luna;

    struct BusinessMan : Node {
        explicit BusinessMan() {
            modelPath = "assets/penger-obj-main/suitger/suitedpenger.obj";
            culling = true;
        }

        void Tick() override {
            transform = glm::rotate(transform, glm::radians(-0.01f), glm::vec3(0.0f, 1.0f, 0.0f));
        }
    };

    auto app = Luna::Initialize(argc, argv);
    SceneManager::CurrentScene().RegisterObject<BusinessMan>();

    app->Run();

    return 0;
}
