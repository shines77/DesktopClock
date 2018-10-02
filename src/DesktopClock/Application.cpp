//
// Application.cpp : main source file for DesktopClock.exe
//

#include "stdafx.h"

#include "resource.h"

#include "Application.h"
#include "MainWnd.h"
#include "AboutDlg.h"

#include <atltypes.h>

CAppModule _Module;

class GDIPlusInitializer {
private:
    ULONG_PTR                    gdiPlusToken_;
    Gdiplus::GdiplusStartupInput gdiPlusStartupInput_;

public:
    GDIPlusInitializer() : gdiPlusToken_(-1) {
    }

    ~GDIPlusInitializer() {
        shutdown();
    }

    ULONG_PTR startup() {
        if (gdiPlusToken_ == -1) {
            Gdiplus::GdiplusStartup(&gdiPlusToken_, &gdiPlusStartupInput_, NULL);
        }

        return gdiPlusToken_;
    }

    void shutdown() {
        if (gdiPlusToken_ != -1) {
            Gdiplus::GdiplusShutdown(gdiPlusToken_);
            gdiPlusToken_ = -1;
        }
    }

    ULONG_PTR getToken() const {
        return gdiPlusToken_;
    }
};

class COMInitializer {
private:
    HRESULT hResult_;
    GDIPlusInitializer gdiPlusInitor_;

public:
    COMInitializer() : hResult_(0) {
        // Initialize COM+
        hResult_ = ::CoInitialize(NULL);

        // Startup GDIPlus
        gdiPlusInitor_.startup();
    }

    COMInitializer(BOOL isComPlus, DWORD dwCoInit = COINIT_MULTITHREADED)
        : hResult_(0) {
        if (isComPlus) {
            // Initialize COM+ advanced
            hResult_ = ::CoInitializeEx(NULL, dwCoInit);
        }
        else {
            // Initialize COM+
            hResult_ = ::CoInitialize(NULL);
        }

        // Startup GDIPlus
        gdiPlusInitor_.startup();
    }

    ~COMInitializer() {
        // Shutdown GDIPlus
        gdiPlusInitor_.shutdown();

        // Uninitialize COM+
        ::CoUninitialize();
    }

    HRESULT getResult() const {
        return hResult_;
    }
};

int Run(LPTSTR lpstrCmdLine = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	MainWnd wndMain;

	if (wndMain.CreateEx() == NULL) {
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

    CRect rcWin(0, 0, 500, 400);
    wndMain.MoveWindow(rcWin, FALSE);
    wndMain.CenterWindow();
	wndMain.ShowWindow(SW_HIDE);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpstrCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);

    COMInitializer comInitor;
    HRESULT hResult = comInitor.getResult();

	// HRESULT hResult = ::CoInitialize(NULL);

    // If you are running on NT 4.0 or higher you can use the following call instead to 
    // make the EXE free threaded. This means that calls come in on a random RPC thread.
    //	HRESULT hResult = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hResult));

    int nRet;
    {
	    // This resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	    ::DefWindowProc(NULL, 0, 0, 0L);

	    AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	    hResult = _Module.Init(NULL, hInstance);
	    ATLASSERT(SUCCEEDED(hResult));

	    nRet = Run(lpstrCmdLine, nCmdShow);
	    _Module.Term();
    }

	return nRet;
}
