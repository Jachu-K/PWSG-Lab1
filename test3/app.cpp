#include "app.h"
#include <windowsx.h>   // dla GET_X_LPARAM, GET_Y_LPARAM
#include <stdexcept>

// --------------------------------------------------------------------
// Sta³e
// --------------------------------------------------------------------
static const std::wstring CLASS_NAME = L"2048WindowClass";
static const std::wstring WINDOW_TITLE = L"2048";

// --------------------------------------------------------------------
// Konstruktor / Destruktor
// --------------------------------------------------------------------
App2048::App2048(HINSTANCE hInstance)
    : m_hInst(hInstance)
    , m_hMainWnd(nullptr)
    , m_className(CLASS_NAME)
    , m_isProcessingMove(false)
{
    if (!registerWindowClass())
        throw std::runtime_error("Failed to register window class");

    m_hMainWnd = createMainWindow();
    if (!m_hMainWnd)
        throw std::runtime_error("Failed to create main window");
}

App2048::~App2048()
{
    // Zwalnianie zasobów (jeœli coœ by³o tworzone, np. pêdzle, ikony)
    // W tym prostym przyk³adzie nic nie trzeba zwalniaæ, ale w docelowej aplikacji:
    // - zniszcz pêdzle (DeleteObject)
    // - zniszcz ikony (DestroyIcon)
}

// --------------------------------------------------------------------
// Rejestracja klasy okna
// --------------------------------------------------------------------
bool App2048::registerWindowClass()
{
    WNDCLASSEXW wc = {};

    // SprawdŸ, czy klasa ju¿ jest zarejestrowana
    if (GetClassInfoExW(m_hInst, m_className.c_str(), &wc) != 0)
        return true; // ju¿ istnieje

    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = windowProcStatic;
    wc.hInstance = m_hInst;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1); // domyœlne t³o
    wc.lpszClassName = m_className.c_str();

    // Opcjonalnie: ustaw ikonê (póŸniej mo¿esz dodaæ LoadImage)
    // wc.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
    // wc.hIconSm = nullptr;

    return RegisterClassExW(&wc) != 0;
}

// --------------------------------------------------------------------
// Tworzenie okna
// --------------------------------------------------------------------
HWND App2048::createMainWindow()
{
    return CreateWindowExW(
        0,                              // rozszerzone style
        m_className.c_str(),            // nazwa klasy
        WINDOW_TITLE.c_str(),           // tytu³
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, // style
        CW_USEDEFAULT, CW_USEDEFAULT,   // pozycja x, y
        640, 480,                       // szerokoœæ, wysokoœæ
        nullptr,                        // rodzic
        nullptr,                        // menu
        m_hInst,                        // instancja
        this                            // parametr dodatkowy (this)
    );
}

// --------------------------------------------------------------------
// Statyczna procedura okna – poœredniczy do niestatycznej
// --------------------------------------------------------------------
LRESULT CALLBACK App2048::windowProcStatic(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    App2048* pThis = nullptr;

    if (uMsg == WM_NCCREATE)
    {
        // Przy tworzeniu okna odczytujemy wskaŸnik this z lpCreateParams
        CREATESTRUCTW* pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
        pThis = static_cast<App2048*>(pCreate->lpCreateParams);

        // Zapisz wskaŸnik w USERDATA okna
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    else
    {
        // Dla pozosta³ych komunikatów odczytaj wskaŸnik z USERDATA
        pThis = reinterpret_cast<App2048*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }

    if (pThis)
        return pThis->windowProc(hwnd, uMsg, wParam, lParam);
    else
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

// --------------------------------------------------------------------
// Niestatyczna procedura okna – w³aœciwa obs³uga komunikatów
// --------------------------------------------------------------------
LRESULT App2048::windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        // ---------- Klawiatura ----------
    case WM_KEYDOWN:
    {
        // wParam – kod wirtualny klawisza
        WCHAR buffer[128];
        wsprintfW(buffer, L"WM_KEYDOWN: klawisz %d (0x%X)", wParam, wParam);
        MessageBoxW(hwnd, buffer, L"Klawiatura", MB_OK);
        return 0;
    }

    case WM_CHAR:
    {
        // wParam – kod znaku Unicode
        WCHAR buffer[128];
        wsprintfW(buffer, L"WM_CHAR: znak '%c' (kod %d)", (WCHAR)wParam, wParam);
        MessageBoxW(hwnd, buffer, L"Klawiatura", MB_OK);
        return 0;
    }

    // ---------- Mysz ----------
    case WM_LBUTTONDOWN:
    {
        // Pozycja kursora w obszarze klienckim
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        WCHAR buffer[128];
        wsprintfW(buffer, L"Lewy przycisk w (%d, %d)", x, y);
        MessageBoxW(hwnd, buffer, L"Mysz", MB_OK);
        return 0;
    }

    case WM_RBUTTONDOWN:
    {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        WCHAR buffer[128];
        wsprintfW(buffer, L"Prawy przycisk w (%d, %d)", x, y);
        MessageBoxW(hwnd, buffer, L"Mysz", MB_OK);
        return 0;
    }

    case WM_MOUSEMOVE:
    {
        // Mo¿esz reagowaæ na ruch myszy – tu tylko przyk³ad wyœwietlenia w tytule
        // (ale czêste odœwie¿anie mo¿e spowalniaæ, wiêc lepiej u¿yæ TrackMouseEvent)
        static int lastX = -1, lastY = -1;
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        if (x != lastX || y != lastY)
        {
            lastX = x; lastY = y;
            WCHAR buffer[256];
            wsprintfW(buffer, L"2048 - mysz (%d, %d)", x, y);
            SetWindowTextW(hwnd, buffer);
        }
        // Nie zwracamy 0, bo domyœlna procedura te¿ mo¿e chcieæ to obs³u¿yæ
        break;
    }

    case WM_MOUSEWHEEL:
    {
        // Delta kó³ka: HIWORD(wParam)
        short delta = GET_WHEEL_DELTA_WPARAM(wParam);
        WCHAR buffer[128];
        wsprintfW(buffer, L"Kó³ko: delta = %d", delta);
        MessageBoxW(hwnd, buffer, L"Mysz", MB_OK);
        return 0;
    }

    // ---------- Zamkniêcie okna ----------
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;

    case WM_DESTROY:
        // G³ówne okno zniszczone – koñczymy pêtlê komunikatów
        if (hwnd == m_hMainWnd)
            PostQuitMessage(0);
        return 0;

        // ---------- Domyœlna obs³uga ----------
    default:
        break;
    }

    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

// --------------------------------------------------------------------
// Uruchomienie pêtli komunikatów
// --------------------------------------------------------------------
int App2048::run(int nShowCmd)
{
    ShowWindow(m_hMainWnd, nShowCmd);
    UpdateWindow(m_hMainWnd);

    MSG msg = {};
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return static_cast<int>(msg.wParam);
}