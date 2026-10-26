#include "windows_main.hpp"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    // windows apps are decoupled from a terminal by default, so we'll spawn one
    if (AllocConsole()) {
        FILE* fp;
        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONOUT$", "w", stderr);
        std::ios::sync_with_stdio();
    }

    spdlog::set_level(spdlog::level::trace);
    spdlog::info("hello");

    return Luna::EngineMain(__argc, __argv);
}
