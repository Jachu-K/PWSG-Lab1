#pragma once
#pragma once

#include <windows.h>
#include <string>

class App2048
{
public:
    explicit App2048(HINSTANCE hInstance);
    ~App2048();

    int run(int nShowCmd);

private:
    // Rejestracja klasy okna
    bool registerWindowClass();

    // Tworzenie g³ównego okna
    HWND createMainWindow();

    // Statyczna procedura okna (do przekazywania wskaŸnika this)
    static LRESULT CALLBACK windowProcStatic(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // Niestatyczna procedura okna – tutaj obs³uga komunikatów
    LRESULT windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // Pomocnicza do pobierania wskaŸnika this z okna
    static App2048* getThisFromWindow(HWND hwnd);

    // Sk³adowe
    HINSTANCE m_hInst;
    HWND      m_hMainWnd;
    std::wstring m_className;

    // Flaga do unikania zapêtleñ (przyda siê przy przesuwaniu okien)
    bool m_isProcessingMove;
};