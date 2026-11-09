#include <spdlog/spdlog.h>

#include "core/AppCore.hpp"
#include "graphics/IRenderer.hpp"

namespace {
class BusinessMan : public Luna::GameObject {
public:
    explicit BusinessMan(Luna::HandleConfiguration config) {
        model = Luna::IModel::Create(config.get(), "assets/penger-obj-main/suitger/suitedpenger.obj");
        culling = true;
    }
};
}

template <typename T>
static void RegisterObject(const std::unique_ptr<Luna::AppCore> &app) {
    Luna::HandleGameObject newObject = std::make_unique<T>(app->config);
    app->RegisterObject(newObject);
}

int main(int argc, char *argv[]) {
    const auto app = Luna::Initialize();

    RegisterObject<BusinessMan>(app);

    app->Run();

    spdlog::info("Exiting successfully");
    return 0;
}
