#include "app.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/,
    LPWSTR /*lpCmdLine*/, int nShowCmd)
{
    try {
        App2048 app(hInstance);
        return app.run(nShowCmd);
    }
    catch (...) {
        MessageBoxW(nullptr, L"Application failed to start.", L"Error", MB_ICONERROR);
        return 1;
    }
}