/////////////////////////////////////////////////////////////////////////////
//
// DesktopClockWnd.h : interface of the DesktopClockWnd class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

//
// Whether use GDI+ image class ?
//
#define USE_GDIPLUS_IMAGE       1

#include <atlwin.h>
#include <atlstr.h>

#include <atlimage.h>
#if USE_GDIPLUS_IMAGE
#include <gdiplus.h>
#endif

using namespace ATL;

#if USE_GDIPLUS_IMAGE
using namespace Gdiplus;
#endif

//
// The timer id of UpdateTime().
//
#define ID_TIMER_UPDATETIME     1

struct SysTime
{
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMillsecs;

    SysTime() : wHour(0), wMinute(0), wSecond(0), wMillsecs(0) {
    }

    ~SysTime() {}
};

class DesktopClockWnd : public CWindowImpl<DesktopClockWnd, ATL::CWindow, ATL::CWinTraits<WS_OVERLAPPED, 0>>,
                        public CUpdateUI<DesktopClockWnd>,
                        public CMessageFilter,
                        public CIdleHandler
{
private:
#if USE_GDIPLUS_IMAGE
    CStringW m_bgImagePath;
    Gdiplus::Image * m_bgImage;
#else
    CString m_bgImagePath;
    ATL::CImage * m_bgImage;
#endif // USE_GDIPLUS_IMAGE
    bool m_bgImageLoaded;

    ULONG_PTR m_nUpdateTimeEvent;
    SYSTEMTIME m_lastTime;

public:
    DECLARE_WND_CLASS(_T("DesktopClockWnd"))

    DesktopClockWnd();
    virtual ~DesktopClockWnd();

    BEGIN_MSG_MAP(DesktopClockWnd)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MESSAGE_HANDLER(WM_TIMER, OnTimer)
    END_MSG_MAP()

    BEGIN_UPDATE_UI_MAP(DesktopClockWnd)
    END_UPDATE_UI_MAP()

    BOOL hasInited() const { return ((m_hWnd != NULL) && ::IsWindow(m_hWnd)); }

    virtual BOOL PreTranslateMessage(MSG * pMsg);
    virtual BOOL OnIdle();

    // Handler prototypes (uncomment arguments if needed):
    //  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
    //  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled);
    //  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL & bHandled);

    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
    LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
    LRESULT OnPaint(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
    LRESULT OnTimer(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);

    void OnFinalMessage(HWND hwnd);

    BOOL NeedUpdateTime(const SYSTEMTIME & sysTime);

    void UpdateTime();

    void RecordLastUpdateTime();
    void RecordLastUpdateTime(const SYSTEMTIME & sysTime);

private:
    void UpdateLayerWindow();
    void UpdateLayerWindow(HDC hDC);

    void DrawShadowText(Gdiplus::Graphics & g, const ATL::CStringW text,
                        const Gdiplus::Font & font,
                        const Gdiplus::RectF & rcLayout,
                        const Gdiplus::StringFormat & strFormat,
                        const Gdiplus::SolidBrush & foreBrush,
                        const Gdiplus::SolidBrush & shadowBrush,
                        Gdiplus::REAL blurScale, Gdiplus::REAL distance);
};
