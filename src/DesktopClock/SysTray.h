
#pragma once

#include <atlwin.h>

class SysTray
{
private:
    BOOL inited_;
    BOOL hooked_;
    NOTIFYICONDATA nid_;

public:
    SysTray();
    ~SysTray();

    static BOOL isValidWindow(HWND hWnd);
    static BOOL isVisible(HWND hWnd);

    BOOL hasInited() const { return inited_; }
    BOOL hasHooked() const { return hooked_; }

    void destroy();
    
    BOOL addIcon(HWND hWnd, HICON hIcon);
    BOOL updateIcon(HICON hIcon);
    BOOL removeIcon();

    BOOL hookWindow(HWND hWnd);
    void unhookWindow();

    static HWND hWndMain;
    static WNDPROC lpPrevWndProc;
    static BOOL isForegroundWindow;

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};
