#define NOMINMAX
#include <windows.h>
#include <vector>
#include <algorithm>
#include <windowsx.h>
// Global variables
std::vector<HWND> g_rectangles;      // saved rectangle windows
HWND g_hActiveRect = nullptr;        // currently dragged rectangle (if any)
POINT g_ptStart = { 0, 0 };           // mouse down position
HBRUSH g_hBrushRect = nullptr;        // brush for rectangle colour (170,70,80)
HBRUSH g_hBrushBackground = nullptr;  // brush for main window background (30,50,90)
HINSTANCE g_hInst = nullptr;          // application instance

// Forward declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, int nCmdShow)
{
    g_hInst = hInstance;

    // Create brushes
    g_hBrushBackground = CreateSolidBrush(RGB(30, 50, 90));
    g_hBrushRect = CreateSolidBrush(RGB(170, 70, 80));

    // Register main window class
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = g_hBrushBackground;
    wc.lpszClassName = L"MainWindowClass";

    if (!RegisterClassEx(&wc))
        return 0;

    // Create main window with fixed client area 800x600
    RECT clientRect = { 0, 0, 800, 600 };
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN;
    AdjustWindowRect(&clientRect, style, FALSE);
    int windowWidth = clientRect.right - clientRect.left;
    int windowHeight = clientRect.bottom - clientRect.top;

    HWND hWnd = CreateWindowEx(
        0,
        L"MainWindowClass",
        L"Not WM_PAINT",
        style,
        CW_USEDEFAULT, CW_USEDEFAULT,
        windowWidth, windowHeight,
        nullptr, nullptr, hInstance, nullptr
    );

    if (!hWnd)
        return 0;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Cleanup brushes
    DeleteObject(g_hBrushRect);
    DeleteObject(g_hBrushBackground);

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_LBUTTONDOWN:
    {
        // If somehow an active rectangle exists, destroy it first
        if (g_hActiveRect)
        {
            DestroyWindow(g_hActiveRect);
            g_hActiveRect = nullptr;
            ReleaseCapture();
        }

        // Store start point
        g_ptStart.x = GET_X_LPARAM(lParam);
        g_ptStart.y = GET_Y_LPARAM(lParam);

        // Create active rectangle as a 1x1 static child
        g_hActiveRect = CreateWindowEx(
            0,
            L"STATIC",
            nullptr,
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            g_ptStart.x, g_ptStart.y, 1, 1,
            hWnd,
            nullptr,
            g_hInst,
            nullptr
        );

        SetCapture(hWnd);
        break;
    }

    case WM_MOUSEMOVE:
    {
        if (g_hActiveRect && (wParam & MK_LBUTTON))
        {
            int curX = GET_X_LPARAM(lParam);
            int curY = GET_Y_LPARAM(lParam);

            int left = (g_ptStart.x < curX) ? g_ptStart.x : curX;
            int top = (g_ptStart.y < curY) ? g_ptStart.y : curY;
            int right = (g_ptStart.x > curX) ? g_ptStart.x : curX;
            int bottom = (g_ptStart.y > curY) ? g_ptStart.y : curY;

            SetWindowPos(g_hActiveRect, nullptr,
                left, top,
                right - left, bottom - top,
                SWP_NOZORDER);
        }
        break;
    }

    case WM_LBUTTONUP:
    {
        if (g_hActiveRect)
        {
            // Save the rectangle
            g_rectangles.push_back(g_hActiveRect);
            g_hActiveRect = nullptr;
            ReleaseCapture();
        }
        break;
    }

    case WM_KEYDOWN:
    {
        if (wParam == VK_BACK)   // Backspace key
        {
            if (g_hActiveRect)
            {
                // Delete active rectangle
                DestroyWindow(g_hActiveRect);
                g_hActiveRect = nullptr;
                ReleaseCapture();
                InvalidateRect(hWnd, nullptr, TRUE);
            }
            else if (!g_rectangles.empty())
            {
                // Delete last saved rectangle
                HWND hLast = g_rectangles.back();
                g_rectangles.pop_back();
                DestroyWindow(hLast);
                InvalidateRect(hWnd, nullptr, TRUE);
            }
        }
        break;
    }

    case WM_CTLCOLORSTATIC:
    {
        // All static controls are our rectangles – return the rectangle brush
        return (LRESULT)g_hBrushRect;
    }

    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }

    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}