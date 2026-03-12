// Template2.cpp – Uniwersalny szablon WinAPI z przykładami
// Komentarze wyjaśniają kluczowe elementy

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>   // dla kontrolek (jeśli potrzebne)
#include <string>

// Stałe identyfikatorów kontrolek (dla WM_COMMAND)
#define IDC_BUTTON_TEST     1001
#define IDC_EDIT_INPUT      1002
#define IDC_STATIC_LABEL    1003
#define IDC_CHILD_WINDOW    1004   // własne okno dziecko

// Globalna instancja
HINSTANCE g_hInst = nullptr;

// Deklaracje procedur okien
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildWndProc(HWND, UINT, WPARAM, LPARAM);

// Struktura do przechowywania danych związanych z oknem (opcjonalnie)
struct WindowData {
    std::wstring someText;
    int counter;
    // ... inne dane
};

// Rejestracja klasy głównego okna
BOOL RegisterMainWindowClass(HINSTANCE hInstance) {
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainWndProc;                 // procedura głównego okna
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);  // domyślne tło
    wc.lpszMenuName = nullptr;                     // można dodać menu
    wc.lpszClassName = L"MainWindowClass";
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
    return RegisterClassEx(&wc) != 0;
}

// Rejestracja klasy dla własnego okna dziecka
BOOL RegisterChildWindowClass(HINSTANCE hInstance) {
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = ChildWndProc;                 // procedura dziecka
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);   // inny kolor tła
    wc.lpszClassName = L"ChildWindowClass";
    return RegisterClassEx(&wc) != 0;
}

// WinMain
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow) {
    g_hInst = hInstance;

    // Inicjalizacja biblioteki Common Controls (jeśli potrzebujemy nowoczesnych kontrolek)
    //INITCOMMONCONTROLSEX icc = { sizeof(INITCOMMONCONTROLSEX), ICC_STANDARD_CLASSES };
    //InitCommonControlsEx(&icc);

    // Rejestracja klas okien
    if (!RegisterMainWindowClass(hInstance) || !RegisterChildWindowClass(hInstance))
        return FALSE;

    // Tworzenie głównego okna
    HWND hWndMain = CreateWindowEx(
        0,
        L"MainWindowClass",
        L"Szablon WinAPI",
        WS_OVERLAPPEDWINDOW,        // standardowe okno z możliwością zmiany rozmiaru
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        nullptr, nullptr, hInstance, nullptr);

    if (!hWndMain)
        return FALSE;

    ShowWindow(hWndMain, nCmdShow);
    UpdateWindow(hWndMain);

    // Pętla komunikatów
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

// ========== Procedura głównego okna ==========
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    // Wskaźnik do danych okna (możemy przechowywać w nim np. uchwyty kontrolek)
    // Na początku (WM_NCCREATE) możemy przydzielić pamięć i zapisać wskaźnik.
    static WindowData* pData = nullptr;   // prosta wersja – zmienna statyczna

    switch (message) {
    case WM_CREATE:
    {
        // Alokacja danych okna (można też użyć SetWindowLongPtr)
        pData = new WindowData{ L"Początkowy tekst", 0 };

        // Tworzymy kilka kontrolek – przykład
        // Przycisk
        CreateWindowEx(0, L"BUTTON", L"Kliknij mnie",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            50, 50, 120, 30,
            hWnd, (HMENU)IDC_BUTTON_TEST, g_hInst, nullptr);

        // Pole edycyjne
        CreateWindowEx(0, L"EDIT", L"Wpisz coś",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
            50, 100, 200, 25,
            hWnd, (HMENU)IDC_EDIT_INPUT, g_hInst, nullptr);

        // Etykieta statyczna
        CreateWindowEx(0, L"STATIC", L"To jest etykieta",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            50, 140, 200, 20,
            hWnd, (HMENU)IDC_STATIC_LABEL, g_hInst, nullptr);

        // Własne okno dziecko (zarejestrowane jako ChildWindowClass)
        CreateWindowEx(0, L"ChildWindowClass", nullptr,
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            300, 50, 200, 150,
            hWnd, (HMENU)IDC_CHILD_WINDOW, g_hInst, nullptr);
    }
    return 0;

    case WM_COMMAND:
    {
        WORD id = LOWORD(wParam);
        WORD code = HIWORD(wParam);   // kod powiadomienia (np. BN_CLICKED)
        HWND hwndCtl = (HWND)lParam;

        switch (id) {
        case IDC_BUTTON_TEST:
            if (code == BN_CLICKED) {
                // Kliknięto przycisk – zmieniamy tekst w etykiecie
                HWND hEdit = GetDlgItem(hWnd, IDC_EDIT_INPUT);
                if (hEdit) {
                    wchar_t buf[128];
                    GetWindowText(hEdit, buf, 128);
                    SetDlgItemText(hWnd, IDC_STATIC_LABEL, buf);
                }
                // Możemy też odświeżyć okno dziecka (wyślijmy do niego prywatną wiadomość)
                HWND hChild = GetDlgItem(hWnd, IDC_CHILD_WINDOW);
                if (hChild) {
                    // Przykład wysłania wiadomości użytkownika
                    SendMessage(hChild, WM_APP + 1, 0, 0);
                }
            }
            break;

            // Obsługa innych kontrolek...
        }
    }
    return 0;

    case WM_PAINT:
    {
        // Przykład rysowania w głównym oknie
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // Rysujemy prostokąt i tekst
        RECT rc;
        GetClientRect(hWnd, &rc);
        HBRUSH hBrush = CreateSolidBrush(RGB(200, 220, 240));
        FillRect(hdc, &rc, hBrush);
        DeleteObject(hBrush);

        // Tekst
        std::wstring text = L"To jest główne okno. Dane: " + (pData ? pData->someText : L"brak");
        TextOut(hdc, 20, 200, text.c_str(), (int)text.length());

        EndPaint(hWnd, &ps);
    }
    return 0;

    case WM_SIZE:
    {
        // Reakcja na zmianę rozmiaru okna – np. przesuwanie kontrolek
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);
        // Przykład: przesuń przycisk w prawo
        HWND hBtn = GetDlgItem(hWnd, IDC_BUTTON_TEST);
        if (hBtn) {
            SetWindowPos(hBtn, nullptr, width - 150, 50, 0, 0,
                SWP_NOSIZE | SWP_NOZORDER);
        }
    }
    return 0;

    case WM_KEYDOWN:
    {
        // Obsługa klawiatury
        if (wParam == VK_F1) {
            MessageBox(hWnd, L"Wciśnięto F1", L"Informacja", MB_OK);
        }
    }
    return 0;

    case WM_DESTROY:
        // Zwalniamy dane okna
        if (pData) {
            delete pData;
            pData = nullptr;
        }
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// ========== Procedura okna dziecka (własna klasa) ==========
LRESULT CALLBACK ChildWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    // Tu też możemy przechowywać dane
    static int drawCounter = 0;

    switch (message) {
    case WM_CREATE:
        drawCounter = 0;
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        RECT rc;
        GetClientRect(hWnd, &rc);
        // Rysujemy coś w zależności od licznika
        HBRUSH hBrush = CreateSolidBrush(RGB(100 + drawCounter * 10, 150, 200));
        FillRect(hdc, &rc, hBrush);
        DeleteObject(hBrush);

        wchar_t buf[64];
        wsprintf(buf, L"Licznik: %d", drawCounter);
        DrawText(hdc, buf, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        EndPaint(hWnd, &ps);
    }
    return 0;

    case WM_APP + 1:   // własna wiadomość
    {
        // Przykład: zwiększ licznik i odśwież
        drawCounter++;
        InvalidateRect(hWnd, nullptr, TRUE);
    }
    return 0;

    case WM_LBUTTONDOWN:
    {
        // Kliknięcie w oknie dziecka
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        std::wstring msg = L"Klik w dziecku: (" + std::to_wstring(x) + L"," + std::to_wstring(y) + L")";
        MessageBox(hWnd, msg.c_str(), L"Dziecko", MB_OK);
    }
    return 0;

    case WM_DESTROY:
        // Dziecko nie kończy programu – tylko sprzątanie
        return 0;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}