/////////////////////////////////////////////////////////////////////////////
//
// MainWnd.h : interface of the MainWnd class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "MainWndView.h"
#include "DesktopClockWnd.h"
#include "SysTray.h"

#include <atlframe.h>

using namespace ATL;
using namespace WTL;

class MainWnd : 
	public CFrameWindowImpl<MainWnd>, 
	public CUpdateUI<MainWnd>,
	public CMessageFilter, public CIdleHandler
{
private:
    DesktopClockWnd wndDesktopClock_;
    HICON hSysTrayIcon_;
    SysTray sysTray_;

public:
    MainWnd();
    ~MainWnd();

	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINWND)

	virtual BOOL PreTranslateMessage(MSG * pMsg);
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(MainWnd)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(MainWnd)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
        COMMAND_ID_HANDLER(ID_SYSTRAY_SHOW, OnSysTrayShow)
        COMMAND_ID_HANDLER(ID_SYSTRAY_HIDE, OnSysTrayHide)
        COMMAND_ID_HANDLER(ID_SYSTRAY_SETTING, OnSysTraySetting)
        COMMAND_ID_HANDLER(ID_SYSTRAY_EXIT, OnSysTrayExit)
		CHAIN_MSG_MAP(CUpdateUI<MainWnd>)
		CHAIN_MSG_MAP(CFrameWindowImpl<MainWnd>)
	END_MSG_MAP()

    // Handler prototypes (uncomment arguments if needed):
    //	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
    //	LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled);
    //	LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL & bHandled);

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);

    LRESULT OnFileNew(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled);
	LRESULT OnFileExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled);
	LRESULT OnAppAbout(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled);

    LRESULT OnSysTrayShow(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled);
    LRESULT OnSysTrayHide(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled);
    LRESULT OnSysTraySetting(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled);
    LRESULT OnSysTrayExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled);
};
