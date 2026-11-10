#include "core/AppCore.hpp"

namespace {

struct BusinessMan : Luna::GameObject {
    explicit BusinessMan() {
        modelPath = "assets/penger-obj-main/suitger/suitedpenger.obj";
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
