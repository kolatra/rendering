#include <iostream>

#include "engine/Application.hpp"

int main() {
    std::cout << "Hello, World!" << std::endl;

    auto *app = new Application();
    app->setBackend(Backend::OpenGL);
    app->run(1280, 720);

    return 0;
}
