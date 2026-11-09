#include <Windows.h>

namespace Luna {
    int EngineMain(int argc, char **argv);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    // windows apps are decoupled from a terminal by default, so we'll spawn one
    if (AllocConsole()) {
        FILE* fp;
        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONOUT$", "w", stderr);
        std::ios::sync_with_stdio();
    }

    return Luna::EngineMain(__argc, __argv);
}
