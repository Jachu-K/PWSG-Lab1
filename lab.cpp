// Template1.cpp – Program rysujący prostokąty jako okna STATIC
// Zadanie: "Not WM_PAINT" – prostokąty są oknami potomnymi, nie użyto GDI ani WM_PAINT
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#include <windows.h>
#include <windowsx.h>   // GET_X_LPARAM, GET_Y_LPARAM
#include <vector>
#include <algorithm>    // std::min, std::max

// Kolory
const COLORREF BG_COLOR = RGB(30, 50, 90);      // tło głównego okna
const COLORREF RECT_COLOR = RGB(170, 70, 80);   // kolor prostokątów

// Globalne dane
HINSTANCE g_hInst = nullptr;
HBRUSH g_hBrushRect = nullptr;                   // pędzel dla prostokątów
std::vector<HWND> g_rects;                       // zapisane prostokąty (już narysowane)
HWND g_hCurrentRect = nullptr;                   // prostokąt właśnie rysowany (przeciągany)
POINT g_ptStart = { 0, 0 };                         // punkt początkowy aktywnego prostokąta

// Deklaracja procedury okna głównego
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Funkcja rejestrująca klasę głównego okna
BOOL RegisterMainWindowClass(HINSTANCE hInstance) {
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;                   // tło będziemy malować w WM_ERASEBKGND
    wc.lpszClassName = L"MainWindowClass";
    return RegisterClassEx(&wc) != 0;
}

// WinMain – punkt wejścia
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow) {
    g_hInst = hInstance;

    if (!RegisterMainWindowClass(hInstance))
        return FALSE;

    // Tworzymy główne okno (stały rozmiar 800x600, bez możliwości zmiany rozmiaru)
    HWND hWnd = CreateWindowEx(
        0,
        L"MainWindowClass",
        L"Not WM_PAINT",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,  // bez WS_THICKFRAME
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
        return FALSE;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Główna pętla komunikatów
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

// Procedura okna głównego
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        // Tworzymy pędzel dla prostokątów
        g_hBrushRect = CreateSolidBrush(RECT_COLOR);
        return 0;

    case WM_ERASEBKGND:
    {
        // Ręczne malowanie tła głównego okna kolorem BG_COLOR
        HDC hdc = (HDC)wParam;
        RECT rc;
        GetClientRect(hWnd, &rc);
        HBRUSH hBrush = CreateSolidBrush(BG_COLOR);
        FillRect(hdc, &rc, hBrush);
        DeleteObject(hBrush);
        return TRUE;    // tło zostało wymazane
    }

    case WM_LBUTTONDOWN:
    {
        // Rozpoczęcie rysowania nowego prostokąta
        if (g_hCurrentRect == nullptr) {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            g_ptStart.x = x;
            g_ptStart.y = y;

            // Tworzymy okno STATIC (dziecko) w punkcie kliknięcia o zerowych wymiarach
            g_hCurrentRect = CreateWindowEx(
                0,
                L"STATIC",          // klasa wbudowana
                nullptr,            // bez tekstu
                WS_CHILD | WS_VISIBLE, // style
                x, y, 0, 0,          // początkowe położenie i rozmiar
                hWnd,
                nullptr,            // bez menu
                g_hInst,
                nullptr);

            // Przechwytujemy mysz, aby otrzymywać WM_MOUSEMOVE nawet poza oknem
            SetCapture(hWnd);
        }
    }
    return 0;

    case WM_MOUSEMOVE:
    {
        if (g_hCurrentRect != nullptr) {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);

            // Obliczamy prostokąt od punktu początkowego do bieżącej pozycji
            int left = MIN(g_ptStart.x, x);
            int top = MIN(g_ptStart.y, y);
            int right = MAX(g_ptStart.x, x);
            int bottom = MAX(g_ptStart.y, y);
            int width = right - left;
            int height = bottom - top;

            // Zmieniamy położenie i rozmiar aktywnego prostokąta
            SetWindowPos(g_hCurrentRect, nullptr,
                left, top, width, height,
                SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }
    return 0;

    case WM_LBUTTONUP:
    {
        if (g_hCurrentRect != nullptr) {
            // Kończymy rysowanie – prostokąt zostaje zapisany
            g_rects.push_back(g_hCurrentRect);
            g_hCurrentRect = nullptr;
            ReleaseCapture();   // zwalniamy przechwyconą mysz
            // (nie trzeba odświeżać tła, system zrobi to automatycznie)
        }
    }
    return 0;

    case WM_KEYDOWN:
        if (wParam == VK_BACK) {   // backspace
            if (g_hCurrentRect != nullptr) {
                // Usuwamy aktywny prostokąt
                DestroyWindow(g_hCurrentRect);
                g_hCurrentRect = nullptr;
                ReleaseCapture();   // na всякий случай
                InvalidateRect(hWnd, nullptr, TRUE);   // wymuś odświeżenie tła
            }
            else if (!g_rects.empty()) {
                // Usuwamy ostatni zapisany prostokąt
                HWND hLast = g_rects.back();
                DestroyWindow(hLast);
                g_rects.pop_back();
                InvalidateRect(hWnd, nullptr, TRUE);   // wymuś odświeżenie tła
            }
        }
        return 0;

    case WM_CTLCOLORSTATIC:
    {
        // Obsługa koloru tła dla statików (naszych prostokątów)
        HWND hwndStatic = (HWND)lParam;
        // Sprawdzamy, czy to jeden z naszych prostokątów
        if (g_hCurrentRect == hwndStatic ||
            std::find(g_rects.begin(), g_rects.end(), hwndStatic) != g_rects.end()) {
            // Zwracamy pędzel z kolorem prostokąta
            return (LRESULT)g_hBrushRect;
        }
        // Dla innych statików (jeśli istnieją) używamy domyślnego tła
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    case WM_DESTROY:
        // Zwalniamy pędzel prostokątów
        if (g_hBrushRect) {
            DeleteObject(g_hBrushRect);
            g_hBrushRect = nullptr;
        }
        PostQuitMessage(0);
        return 0;

    case WM_GETMINMAXINFO:
    {
        // Blokujemy zmianę rozmiaru okna (stałe 800x600)
        MINMAXINFO* mmi = (MINMAXINFO*)lParam;
        mmi->ptMinTrackSize.x = 800;
        mmi->ptMinTrackSize.y = 600;
        mmi->ptMaxTrackSize.x = 800;
        mmi->ptMaxTrackSize.y = 600;
    }
    return 0;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}