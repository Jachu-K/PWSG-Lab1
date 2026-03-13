#include <windows.h>

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 500
#define WINDOW_TITLE "Glowne okno"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT rc;
            GetClientRect(hwnd, &rc);
            FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW+1));
            

            EndPaint(hwnd, &ps);
            break;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    const char CLASS_NAME[] = "MainWindowClass";

    WNDCLASS wc = {0};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClass(&wc))
    {
        MessageBox(NULL, "Nie można zarejestrować klasy okna!", "Błąd", MB_ICONERROR);
        return 0;
    }

    HWND hwnd = CreateWindowEx(
        0,                          // rozszerzone style
        CLASS_NAME,                  // nazwa klasy
        WINDOW_TITLE,                // tytuł okna
        WS_OVERLAPPEDWINDOW,         // styl okna
        CW_USEDEFAULT,               // pozycja x
        CW_USEDEFAULT,               // pozycja y
        WINDOW_WIDTH,                // szerokość
        WINDOW_HEIGHT,               // wysokość
        NULL,                        // okno nadrzędne
        NULL,                        // menu
        hInstance,                   // instancja aplikacji
        NULL                         // dane dodatkowe
    );

    if (!hwnd)
    {
        MessageBox(NULL, "Nie można utworzyć okna!", "Błąd", MB_ICONERROR);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
