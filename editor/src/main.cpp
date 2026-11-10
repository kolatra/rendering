#include <spdlog/spdlog.h>

#include "core/AppCore.hpp"
#include "core/Config.hpp"
#include "graphics/IRenderer.hpp"

namespace {
class BusinessMan : public Luna::GameObject {
public:
    explicit BusinessMan(const std::shared_ptr<Luna::Configuration> &config) {
        model = Luna::IModel::Create(config, "assets/penger-obj-main/suitger/suitedpenger.obj");
        culling = true;
    }
};
}

int main(int argc, char **argv) {
    bool storeDefaultConfig = false;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--save-config") == 0) {
            storeDefaultConfig = true;
        }
    }
    const auto app = Luna::Initialize(storeDefaultConfig);
    app->config->showFloor = false;

    app->RegisterObject<BusinessMan>();

    return app->Run();
}
