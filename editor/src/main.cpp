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

/*
 * The engine can be initialized from our main function like so, and objects can be easily registered.
 */
int main(int argc, char **argv) {
    const auto app = Luna::Initialize(argc, argv);

    app->RegisterObject<BusinessMan>();

    return app->Run();
}
