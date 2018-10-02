
#include "stdafx.h"
#include "resource.h"

#include "SysTray.h"

#include <atltypes.h>

#define WM_NOTIFYICON           (WM_USER + 1)
#define WM_DELETE_SYSTRAY_ICON  (WM_USER + 2)

#define SYSTRAY_TIP_TEXT        _T("Desktop Clock")

HWND    SysTray::hWndMain = NULL;
WNDPROC SysTray::lpPrevWndProc = NULL;
BOOL    SysTray::isForegroundWindow = FALSE;

SysTray::SysTray() : inited_(FALSE), hooked_(FALSE)
{
    memset(&nid_, 0, sizeof(nid_));
}

SysTray::~SysTray()
{
    destroy();
}

void SysTray::destroy()
{
    removeIcon();
}

BOOL SysTray::isValidWindow(HWND hWnd)
{
    return ((hWnd != NULL) && ::IsWindow(hWnd));
}

BOOL SysTray::isVisible(HWND hWnd)
{
    return ::IsWindowVisible(hWnd);
}

BOOL SysTray::hookWindow(HWND hWnd)
{
    BOOL result = FALSE;
    if (SysTray::isValidWindow(hWnd)) {
        if (SysTray::lpPrevWndProc != NULL) {
            if (SysTray::isValidWindow(SysTray::hWndMain)) {
                LONG_PTR lResult = ::SetWindowLongPtr(SysTray::hWndMain, GWL_WNDPROC, (LONG)SysTray::lpPrevWndProc);
            }
        }
        
        WNDPROC lpPrevWndProc = (WNDPROC)::GetWindowLongPtr(hWnd, GWL_WNDPROC);
        if (lpPrevWndProc != NULL) {
            LONG_PTR lResult = ::SetWindowLongPtr(hWnd, GWL_WNDPROC, (LONG_PTR)SysTray::WindowProc);
            if (lResult != 0) {
                SysTray::lpPrevWndProc = lpPrevWndProc;
                SysTray::hWndMain = hWnd;
                result = TRUE;
            }
            else {
                DWORD dwError = GetLastError();
            }
        }
    }
    return result;
}

void SysTray::unhookWindow()
{
    if (SysTray::isValidWindow(SysTray::hWndMain)) {
        if (SysTray::lpPrevWndProc != NULL) {
            LONG_PTR result = ::SetWindowLong(hWndMain, GWL_WNDPROC, (LONG_PTR)SysTray::lpPrevWndProc);
            SysTray::lpPrevWndProc = NULL;
        }
        SysTray::hWndMain = NULL;
    }
}

BOOL SysTray::addIcon(HWND hWnd, HICON hIcon)
{
    if (!inited_) {
        if (SysTray::isValidWindow(hWnd)) {
            nid_.cbSize = sizeof (NOTIFYICONDATA);
            nid_.hWnd = hWnd;
            nid_.uID = IDR_MAINFRAME;
            nid_.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
            nid_.uCallbackMessage = WM_NOTIFYICON;
            nid_.hIcon = hIcon;

            errno_t err = _tcscpy_s(nid_.szTip, SYSTRAY_TIP_TEXT);
            BOOL result = Shell_NotifyIcon(NIM_ADD, &nid_);
            inited_ = result;
        }
    }

    if (!hooked_) {
        if (::IsWindow(hWnd)) {
            BOOL success = hookWindow(hWnd);
            if (success) {
                hooked_ = success;
            }
        }
    }
    
    return inited_;
}

BOOL SysTray::updateIcon(HICON hIcon)
{
    if (inited_) {
        if (SysTray::isValidWindow(SysTray::hWndMain)) {
            nid_.cbSize = sizeof (NOTIFYICONDATA);
            nid_.hWnd = SysTray::hWndMain;
            nid_.uID = IDR_MAINFRAME;
            nid_.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
            nid_.uCallbackMessage = WM_NOTIFYICON;
            nid_.hIcon = hIcon;

            errno_t err = _tcscpy_s(nid_.szTip, SYSTRAY_TIP_TEXT);
            BOOL result = Shell_NotifyIcon(NIM_MODIFY, &nid_);
            inited_ = result;
            return result;
        }
    }
    else {
        // Have not add icon yet.
    }

    return FALSE;
}

BOOL SysTray::removeIcon()
{
    if (hooked_) {
        unhookWindow();
        hooked_ = FALSE;
    }

    if (inited_) {
        BOOL result = Shell_NotifyIcon(NIM_DELETE, &nid_);
        if (result) {
            inited_ = FALSE;
        }
    }

    return inited_;
}

LRESULT CALLBACK SysTray::WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    BOOL handled = FALSE;

    switch (message) {
    case WM_NOTIFYICON:
        if (lParam == WM_LBUTTONDBLCLK) {
            if (SysTray::isValidWindow(SysTray::hWndMain)) {
                if (::IsIconic(SysTray::hWndMain) || !::IsWindowVisible(SysTray::hWndMain)) {
                    ::ShowWindow(SysTray::hWndMain, SW_SHOWNORMAL);

                    // Call SetForegroundWindow() for let the menu auto lost the focus and close.
                    ::SetForegroundWindow(SysTray::hWndMain);

                    SysTray::isForegroundWindow = TRUE;
                }
                else {
                    if ((::GetForegroundWindow() == SysTray::hWndMain) || SysTray::isForegroundWindow) {
                        ::ShowWindow(SysTray::hWndMain, SW_HIDE);

                        SysTray::isForegroundWindow = FALSE;
                    }
                    else {
                        // Call SetForegroundWindow() for let the menu auto lost the focus and close.
                        ::SetForegroundWindow(SysTray::hWndMain);

                        SysTray::isForegroundWindow = TRUE;
                    }
                }
            }
        }
        else if (lParam == WM_RBUTTONDOWN) {
            // TODO: Mouse right button down.
        }
        else if (lParam == WM_RBUTTONUP) {
            CPoint pos;
            GetCursorPos(&pos);

            // Load the SysTray menu.
            CMenu menu;
            menu.LoadMenu(IDR_SYSTRAY_MEMU);
            CMenuHandle sysTrayMenu = menu.GetSubMenu(0);

            if (SysTray::isValidWindow(SysTray::hWndMain)) {
                if (!SysTray::isVisible(SysTray::hWndMain)) {
                    // Call SetForegroundWindow() for let the menu auto lost the focus and close.
                    ::SetForegroundWindow(SysTray::hWndMain);
                }

                //::SetCapture(SysTray::hWndMain);

                // Popup the SysTray menu.
                sysTrayMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x, pos.y, SysTray::hWndMain);

                //::ReleaseCapture();
            }
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_SYSTRAY_EXIT:
            if (SysTray::isValidWindow(SysTray::hWndMain)) {
                ::SendMessage(SysTray::hWndMain, WM_CLOSE, 0, 0);
            }
            break;
        }
        break;

    case WM_SYSCOMMAND:
        switch (LOWORD(wParam)) {
        case SC_MINIMIZE:
            //handled = TRUE;
            if (SysTray::isValidWindow(SysTray::hWndMain)) {
                ::ShowWindow(SysTray::hWndMain, SW_HIDE);
            }
            break;

        case SC_CLOSE:
            if (SysTray::isValidWindow(SysTray::hWndMain)) {
                ::SendMessage(SysTray::hWndMain, WM_DELETE_SYSTRAY_ICON, 0, 0);
            }
            break;
        }
        break;

    default:
        if (lpPrevWndProc != NULL)
            result = ::CallWindowProc(lpPrevWndProc, hwnd, message, wParam, lParam);
        else
            result = ::DefWindowProc(hwnd, message, wParam, lParam);
        return result;
    }

    // If handled is FALSE will be continue to call the next hooker.
    if (!handled) {
        if (lpPrevWndProc != NULL)
            result = ::CallWindowProc(lpPrevWndProc, hwnd, message, wParam, lParam);
        else
            result = ::DefWindowProc(hwnd, message, wParam, lParam);
    }

    return result;
}
