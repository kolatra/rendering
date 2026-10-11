#include <iostream>

#include "engine/Application.hpp"

int main() {
    std::string title = "video";
    auto *app = new Application(1280, 720, title);

    app->run();

    return 0;
}
