/////////////////////////////////////////////////////////////////////////////
//
// MainFrame.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "MainWnd.h"
#include "AboutDlg.h"

MainWnd::MainWnd() : hSysTrayIcon_(NULL)
{
    //
}

MainWnd::~MainWnd()
{
    if (hSysTrayIcon_ != NULL) {
        ::DestroyIcon(hSysTrayIcon_);
        hSysTrayIcon_ = NULL;
    }
}

BOOL MainWnd::PreTranslateMessage(MSG * pMsg)
{
    // TODO: Prepare translate the message of CMainWnd windows.

    return CFrameWindowImpl<MainWnd>::PreTranslateMessage(pMsg);
}

BOOL MainWnd::OnIdle()
{
	return FALSE;
}

LRESULT MainWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
    UNREFERENCED_PARAMETER(uMsg);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(bHandled);

    // Set app icons.
    HICON hIcon = AtlLoadIconImage(IDR_MAINWND, LR_DEFAULTCOLOR,
                                   ::GetSystemMetrics(SM_CXICON),
                                   ::GetSystemMetrics(SM_CYICON));
    SetIcon(hIcon, TRUE);

    HICON hIconSmall = AtlLoadIconImage(IDR_MAINWND, LR_DEFAULTCOLOR,
                                        ::GetSystemMetrics(SM_CXSMICON),
                                        ::GetSystemMetrics(SM_CYSMICON));
    SetIcon(hIconSmall, FALSE);

	// Register object for message filtering and idle updates
	CMessageLoop * pMsgLooper = _Module.GetMessageLoop();
	ATLASSERT(pMsgLooper != NULL);
	pMsgLooper->AddMessageFilter(this);
	pMsgLooper->AddIdleHandler(this);

    SetWindowText(_T("Desktop Clock"));

    // Add to Sys-Tray icon.
    hSysTrayIcon_ = AtlLoadIconImage(IDR_MAINWND, LR_DEFAULTCOLOR,
                                     ::GetSystemMetrics(SM_CXICON),
                                     ::GetSystemMetrics(SM_CYICON));
    ATLASSERT(hSysTrayIcon_ != NULL);
    if (hSysTrayIcon_ != NULL) {
        BOOL success = sysTray_.addIcon(m_hWnd, hSysTrayIcon_);
    }

    // Center the dialog on the screen.
    //CenterWindow();

    // Create the DesktopClockWnd window.
    if (!wndDesktopClock_.hasInited()) {
        CRect rect;
        GetWindowRect(&rect);
        rect.right = rect.left + 500 + 8;
        rect.bottom = rect.top + 500 + 8;

        wndDesktopClock_.Create(::GetDesktopWindow(), rect, _T("DesktopClockWnd"),
                                WS_POPUP | WS_VISIBLE, WS_EX_LAYERED | WS_EX_TOOLWINDOW);

        wndDesktopClock_.ShowWindow(SW_SHOWNOACTIVATE);
        wndDesktopClock_.UpdateWindow();

        wndDesktopClock_.UpdateTime();
    }

	return 0;
}

LRESULT MainWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
    UNREFERENCED_PARAMETER(uMsg);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);

    if (wndDesktopClock_.hasInited()) {
        wndDesktopClock_.DestroyWindow();
    }

    if (sysTray_.hasInited()) {
        sysTray_.removeIcon();
    }

	// Unregister message filtering and idle updates
	CMessageLoop * pMsgLooper = _Module.GetMessageLoop();
	ATLASSERT(pMsgLooper != NULL);
	pMsgLooper->RemoveMessageFilter(this);
	pMsgLooper->RemoveIdleHandler(this);

	bHandled = FALSE;
	return 1;
}

LRESULT MainWnd::OnFileExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled)
{
    UNREFERENCED_PARAMETER(wNotifyCode);
    UNREFERENCED_PARAMETER(wID);
    UNREFERENCED_PARAMETER(hWndCtl);
    UNREFERENCED_PARAMETER(bHandled);

	this->PostMessage(WM_CLOSE);
	return 0;
}

LRESULT MainWnd::OnFileNew(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled)
{
    UNREFERENCED_PARAMETER(wNotifyCode);
    UNREFERENCED_PARAMETER(wID);
    UNREFERENCED_PARAMETER(hWndCtl);
    UNREFERENCED_PARAMETER(bHandled);

	// TODO: add code to initialize document

	return 0;
}

LRESULT MainWnd::OnAppAbout(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled)
{
    UNREFERENCED_PARAMETER(wNotifyCode);
    UNREFERENCED_PARAMETER(wID);
    UNREFERENCED_PARAMETER(hWndCtl);
    UNREFERENCED_PARAMETER(bHandled);

	AboutDlg dlg;
	dlg.DoModal();

	return 0;
}

LRESULT MainWnd::OnSysTrayShow(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled)
{
    UNREFERENCED_PARAMETER(wNotifyCode);
    UNREFERENCED_PARAMETER(wID);
    UNREFERENCED_PARAMETER(hWndCtl);
    UNREFERENCED_PARAMETER(bHandled);

    ShowWindow(SW_SHOWNORMAL);
    return 0;
}

LRESULT MainWnd::OnSysTrayHide(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled)
{
    UNREFERENCED_PARAMETER(wNotifyCode);
    UNREFERENCED_PARAMETER(wID);
    UNREFERENCED_PARAMETER(hWndCtl);
    UNREFERENCED_PARAMETER(bHandled);

    ShowWindow(SW_HIDE);
    return 0;
}

LRESULT MainWnd::OnSysTraySetting(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled)
{
    UNREFERENCED_PARAMETER(wNotifyCode);
    UNREFERENCED_PARAMETER(wID);
    UNREFERENCED_PARAMETER(hWndCtl);
    UNREFERENCED_PARAMETER(bHandled);

    return 0;
}

LRESULT MainWnd::OnSysTrayExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled)
{
    UNREFERENCED_PARAMETER(wNotifyCode);
    UNREFERENCED_PARAMETER(hWndCtl);
    UNREFERENCED_PARAMETER(bHandled);

    this->PostMessage(WM_CLOSE);
    return 0;
}
