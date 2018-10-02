
#include "stdafx.h"

#include "DesktopClockWnd.h"
#include "AtlRect.h"

#include <atltypes.h>

using namespace ATL;

#if USE_GDIPLUS_IMAGE
using namespace Gdiplus;
#endif

//
// Whether use Chinese version ?
//
#define USE_CHINESE_TEXT        1

//
// Whether draw the clock text or number ?
//
#define DRAW_CLOCK_TEXT         1

//
// Whether draw the text rectangle ?
//
#define DRAW_TEXT_RECT          0

static
const WCHAR * GetDayOfWeekString(int dayOfWeek)
{
#if USE_CHINESE_TEXT
    static const WCHAR * s_DayOfWeek[] = {
        L"星期天",
        L"星期一",
        L"星期二",
        L"星期三",
        L"星期四",
        L"星期五",
        L"星期六"
    };
#else
    static const WCHAR * s_DayOfWeek[] = {
        L"Sunday",
        L"Monday",
        L"Tuesday",
        L"Wednesday",
        L"Thursday",
        L"Friday",
        L"Saturday"
    };
#endif // USE_CHINESE_TEXT

    ATLASSERT(dayOfWeek >= 0 && dayOfWeek < 7);
    return s_DayOfWeek[dayOfWeek];
}

static
const WCHAR * GetMonthString(int month)
{
#if USE_CHINESE_TEXT
    static const WCHAR * s_Month[] = {
        L"未知",
        L"一月",
        L"二月",
        L"三月",
        L"四月",
        L"五月",
        L"六月",
        L"七月",
        L"八月",
        L"九月",
        L"十月",
        L"十一月",
        L"十二月"
    };
#else
    static const WCHAR * s_Month[] = {
        L"Unknown",
        L"January",
        L"February",
        L"March",
        L"April",
        L"May",
        L"June",
        L"July",
        L"August",
        L"September",
        L"October",
        L"November",
        L"December"
    };
#endif // USE_CHINESE_TEXT

    ATLASSERT(month >= 1 && month <= 12);
    return s_Month[month];
}

DesktopClockWnd::DesktopClockWnd() : m_bgImage(NULL), m_nUpdateTimeEvent(0)
{
#if USE_GDIPLUS_IMAGE
#ifndef NDEBUG
    m_bgImagePath = L"C:\\Project\\shines77\\gxDesktopClock\\Release\\background.png";
#else
    m_bgImagePath = L"background.png";
#endif

    m_bgImage = Gdiplus::Image::FromFile(m_bgImagePath.GetString(), FALSE);
    if (m_bgImage != NULL)
        m_bgImageLoaded = TRUE;
    else
        m_bgImageLoaded = FALSE;
#else
#ifndef NDEBUG
    m_bgImagePath = _T("C:\\Project\\shines77\\gxDesktopClock\\Release\\background.bmp");
#else
    m_bgImagePath = _T("background.bmp");
#endif

    m_bgImage = new ATL::CImage();
    if (m_bgImage != NULL) {
        if (SUCCEEDED(m_bgImage.Load(m_bgImagePath)))
            m_bgImageLoaded = TRUE;
        else
            m_bgImageLoaded = FALSE;
    }
    else {
        m_bgImageLoaded = FALSE;
    }
#endif    
}

DesktopClockWnd::~DesktopClockWnd()
{
    if (m_bgImage != NULL) {
#if USE_GDIPLUS_IMAGE
        delete m_bgImage;
#else
        m_bgImage->Destroy();
        delete m_bgImage;
#endif
        m_bgImage = NULL;
    }

    m_bgImageLoaded = FALSE;
}

BOOL DesktopClockWnd::PreTranslateMessage(MSG * pMsg)
{
    return CWindow::IsDialogMessage(pMsg);
}

BOOL DesktopClockWnd::OnIdle()
{
    UIUpdateChildWindows();
    return FALSE;
}

LRESULT DesktopClockWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
    UNREFERENCED_PARAMETER(uMsg);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(bHandled);

    ATLASSERT(m_hWnd != NULL);
    m_nUpdateTimeEvent = ::SetTimer(m_hWnd, ID_TIMER_UPDATETIME, 5 * 1000, NULL);
    ATLASSERT(m_nUpdateTimeEvent != 0);

    bHandled = FALSE;
    return 0;
}

LRESULT DesktopClockWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
    UNREFERENCED_PARAMETER(uMsg);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(bHandled);

    if (m_nUpdateTimeEvent != 0) {
        ATLASSERT(m_hWnd != NULL);
        ATLASSERT(::IsWindow(m_hWnd));
        ::KillTimer(m_hWnd, m_nUpdateTimeEvent);
        m_nUpdateTimeEvent = 0;
    }

    bHandled = FALSE;
    return 0;
}

LRESULT DesktopClockWnd::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
    UNREFERENCED_PARAMETER(uMsg);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);

    PAINTSTRUCT ps;
    HDC hDC = ::GetDC(m_hWnd);
    if (hDC != NULL) {
        ::BeginPaint(m_hWnd, &ps);

        // Update date-time info now!
        this->UpdateLayerWindow(hDC);

        ::EndPaint(m_hWnd, &ps);
        ::ReleaseDC(m_hWnd, hDC);
    }
    else {
        ::BeginPaint(m_hWnd, &ps);
        ::EndPaint(m_hWnd, &ps);
    }

    bHandled = TRUE;
    return 0;
}

LRESULT DesktopClockWnd::OnTimer(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
    switch (wParam) {
    case ID_TIMER_UPDATETIME:
        this->UpdateTime();
        break;

    default:
        break;
    }

    bHandled = TRUE;
    return 0;
}

void DesktopClockWnd::OnFinalMessage(HWND hwnd)
{
    ::PostQuitMessage(0);
}

BOOL DesktopClockWnd::NeedUpdateTime(const SYSTEMTIME & sysTime)
{
    if (sysTime.wMinute != m_lastTime.wMinute ||
        sysTime.wHour != m_lastTime.wHour ||
        sysTime.wDay != m_lastTime.wDay ||
        sysTime.wDayOfWeek != m_lastTime.wDayOfWeek ||
        sysTime.wMonth != m_lastTime.wMonth) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

void DesktopClockWnd::RecordLastUpdateTime(const SYSTEMTIME & sysTime)
{
    // Record the last date-time
    m_lastTime.wMonth       = sysTime.wMonth;
    m_lastTime.wDayOfWeek   = sysTime.wDayOfWeek;
    m_lastTime.wDay         = sysTime.wDay;
    m_lastTime.wHour        = sysTime.wHour;
    m_lastTime.wMinute      = sysTime.wMinute;
    m_lastTime.wSecond      = sysTime.wSecond;
}

void DesktopClockWnd::RecordLastUpdateTime()
{
    SYSTEMTIME sysTime;
    ::GetLocalTime(&sysTime);

    // Record the last date-time
    RecordLastUpdateTime(sysTime);
}

void DesktopClockWnd::UpdateTime()
{
    SYSTEMTIME sysTime;
    ::GetLocalTime(&sysTime);

    if (NeedUpdateTime(sysTime)) {
        // Update date-time info now!
        UpdateLayerWindow();
    }
}

static
Gdiplus::SizeF GetTextBounds(const Gdiplus::Font & font,
                             const Gdiplus::StringFormat & strFormat,
                             const CStringW & text)
{
    Gdiplus::GraphicsPath graphicsPath;
    Gdiplus::FontFamily fontfamily;

    font.GetFamily(&fontfamily);
    graphicsPath.AddString(text.GetString(), -1, &fontfamily, font.GetStyle(),
                           font.GetSize(), Gdiplus::PointF(0.0, 0.0), &strFormat);

    // Get the edge bound.
    Gdiplus::RectF rcBound;   
    graphicsPath.GetBounds(&rcBound);

    // Return the text width and height.
    return Gdiplus::SizeF(rcBound.Width, rcBound.Height);
}

static
Gdiplus::PointF GetRectTopLeft(const Gdiplus::RectF & rect)
{
    return Gdiplus::PointF(rect.X, rect.Y);
}

//
// See: http://blog.sina.com.cn/s/blog_4ffbc45f0101mnwh.html
// See: https://blog.csdn.net/aoshilang2249/article/details/45153375
//
void DesktopClockWnd::DrawShadowText(Gdiplus::Graphics & g, const ATL::CStringW text,
                                     const Gdiplus::Font & font,
                                     const Gdiplus::RectF & rcLayout,
                                     const Gdiplus::StringFormat & strFormat,
                                     const Gdiplus::SolidBrush & foreBrush,
                                     const Gdiplus::SolidBrush & shadowBrush,
                                     Gdiplus::REAL blurScale, Gdiplus::REAL distance)
{
    ATLASSERT(blurScale <= 1.0f);
    // Make a small bitmap.
    Gdiplus::Bitmap bmpShadow((INT)(rcLayout.Width * blurScale), (INT)(rcLayout.Height * blurScale), &g);

    // Get a graphics object for it.
    Gdiplus::Graphics * shadowGraph = Gdiplus::Graphics::FromImage(&bmpShadow);
    if (shadowGraph != NULL) {
        // Must use an anti-aliased rendering hint.
        shadowGraph->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

        // This matrix zooms the text out to blurScale size
        // and offsets it by a little right and down by distance.
        Gdiplus::Matrix matShadow(blurScale, 0.0f, 0.0f, blurScale, distance, distance);
        shadowGraph->SetTransform(&matShadow);

        Gdiplus::PointF posShadow(0.0f, 0.0f);
        // The shadow is drawn.
        shadowGraph->DrawString(text.GetString(), -1, &font, posShadow, NULL, &shadowBrush);

        // The destination Graphics uses a high quality mode.
        g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        // And draws anti-aliased text for accurate fitting.
        g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

        // The small image is blown up to fill the main client rectangle.
        g.DrawImage(&bmpShadow, rcLayout, 0.0f, 0.0f, (Gdiplus::REAL)bmpShadow.GetWidth(),
                    (Gdiplus::REAL)bmpShadow.GetHeight(), Gdiplus::UnitPixel);
        // Finally, the text is drawn on top.
        g.DrawString(text.GetString(), -1, &font, GetRectTopLeft(rcLayout), &strFormat, &foreBrush);

        delete shadowGraph;
    }
}

void DesktopClockWnd::UpdateLayerWindow(HDC hDC)
{
    if (m_hWnd == NULL)
        return;

    if ((!m_bgImageLoaded) || (m_bgImage == NULL))
        return;

    CRect rcClient;
    GetClientRect(&rcClient);

    ATLASSERT(hDC != NULL);
    HDC hMemDC = ::CreateCompatibleDC(hDC);

#if USE_GDIPLUS_IMAGE
    int nImageWidth  = (rcClient.Width()  >= (int)m_bgImage->GetWidth() ) ? rcClient.Width()  : (int)m_bgImage->GetWidth();
    int nImageHeight = (rcClient.Height() >= (int)m_bgImage->GetHeight()) ? rcClient.Height() : (int)m_bgImage->GetHeight();
#else
    int nImageWidth  = (rcClient.Width()  >= m_bgImage->GetWidth() ) ? rcClient.Width()  : m_bgImage->GetWidth();
    int nImageHeight = (rcClient.Height() >= m_bgImage->GetHeight()) ? rcClient.Height() : m_bgImage->GetHeight();
#endif // USE_GDIPLUS_IMAGE

    BITMAPINFO bmpInfo;
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biBitCount = 32;
    bmpInfo.bmiHeader.biHeight = nImageWidth;
    bmpInfo.bmiHeader.biWidth  = nImageHeight;
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biCompression = BI_RGB;
    bmpInfo.bmiHeader.biXPelsPerMeter = 0;
    bmpInfo.bmiHeader.biYPelsPerMeter = 0;
    bmpInfo.bmiHeader.biClrUsed = 0;
    bmpInfo.bmiHeader.biClrImportant = 0;
    bmpInfo.bmiHeader.biSizeImage = bmpInfo.bmiHeader.biWidth * bmpInfo.bmiHeader.biHeight * bmpInfo.bmiHeader.biBitCount / 8;
    HBITMAP hBitmap = ::CreateDIBSection(hMemDC, &bmpInfo, 0, NULL, 0, 0);
    HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);

    Gdiplus::Graphics graph(hMemDC);

#if USE_GDIPLUS_IMAGE
    int nDrawWidth  = (rcClient.Width()  <= (int)m_bgImage->GetWidth() ) ? rcClient.Width()  : (int)m_bgImage->GetWidth();
    int nDrawHeight = (rcClient.Height() <= (int)m_bgImage->GetHeight()) ? rcClient.Height() : (int)m_bgImage->GetHeight();

    ATLASSERT(m_bgImage != NULL);

    // Draw image
    graph.DrawImage(m_bgImage, 0.0, 0.0, (Gdiplus::REAL)nDrawWidth, (Gdiplus::REAL)nDrawHeight);
#else
    int nDrawWidth  = (rcClient.Width()  <= m_bgImage->GetWidth() ) ? rcClient.Width()  : m_bgImage->GetWidth();
    int nDrawHeight = (rcClient.Height() <= m_bgImage->GetHeight()) ? rcClient.Height() : m_bgImage->GetHeight();

    ATLASSERT(m_bgImage != NULL);

    // Draw image
    m_bgImage->Draw(hMemDC, 0, 0, nDrawWidth, nDrawHeight, 0, 0, nDrawWidth, nDrawHeight);
#endif // USE_GDIPLUS_IMAGE

#if DRAW_CLOCK_TEXT
    Gdiplus::RectF rcLayout((Gdiplus::REAL)rcClient.left, (Gdiplus::REAL)rcClient.top,
                            (Gdiplus::REAL)rcClient.Width(), (Gdiplus::REAL)rcClient.Height());
    Gdiplus::RectF rcBound;

    Gdiplus::FontFamily fontFamily(L"Microsoft YaHei UI Light");
    //Gdiplus::FontFamily fontFamily(L"Microsoft NeoGothic");
    //Gdiplus::FontFamily fontFamily(L"Tahoma");
    //Gdiplus::FontFamily fontFamily(L"MS Serif");
    //Gdiplus::FontFamily fontFamily(L"MS Sans Serif");
    //Gdiplus::FontFamily fontFamily(L"MS Gothic");
    //Gdiplus::FontFamily fontFamily(L"Yu Gothic UI");
    //Gdiplus::FontFamily fontFamily(L"Harrington");
    //Gdiplus::FontFamily fontFamily(L"Microsoft YaHei Light");

    // Highlight text
    Gdiplus::REAL emSizeTime = 88.0f;
    Gdiplus::Font fntTime(&fontFamily, emSizeTime, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

    Gdiplus::REAL emSizeNoon = 48.0f;
    Gdiplus::Font fntNoon(&fontFamily, emSizeNoon, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

    Gdiplus::REAL emSizeWeek = 32.0f;
    Gdiplus::Font fntWeek(&fontFamily, emSizeWeek, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

    Gdiplus::REAL emSizeDate = 32.0f;
    Gdiplus::Font fntDate(&fontFamily, emSizeDate, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

    Gdiplus::SolidBrush solidBrush(Gdiplus::Color(255, 255, 255, 255));
    Gdiplus::SolidBrush shadowBrush(Gdiplus::Color(96, 0, 0, 0));
#if DRAW_TEXT_RECT
    Gdiplus::SolidBrush rectBrush(Gdiplus::Color(255, 255, 0, 0));
    Gdiplus::Pen rectPen(&rectBrush, 1.0);
#endif

	Gdiplus::StringFormat strFormat;
    // Left alignment
	strFormat.SetAlignment(Gdiplus::StringAlignmentNear);
    // Top alignment
	strFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
    // No string trimming
    strFormat.SetTrimming(Gdiplus::StringTrimmingNone);
    // Set text tendering to HintAntiAlias
	graph.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

    SYSTEMTIME sysTime;
    ::GetLocalTime(&sysTime);

    // Time: 12:38
    ATL::CStringW strText;
    UINT nHour = (UINT)sysTime.wHour;
    if (nHour >= 12) {
        nHour -= 12;
    }
    if (nHour == 0) {
        nHour = 12;
    }
    strText.Format(L"%02u:%02u", nHour, (UINT)sysTime.wMinute);

    graph.MeasureString(strText.GetString(), strText.GetLength(), &fntTime, GetRectTopLeft(rcLayout), &strFormat, &rcBound);
    //graph.DrawString(strText.GetString(), strText.GetLength(), &fntTime, rcLayout, &strFormat, &solidBrush);

    // Draw the shadow text
    DrawShadowText(graph, strText, fntTime, rcLayout, strFormat, solidBrush, shadowBrush, 1.0f, 2.0f);

#if DRAW_TEXT_RECT
    Gdiplus::RectF rcRect;

    // Draw time rectangle
    rcRect.X = rcLayout.X;
    rcRect.Y = rcLayout.Y;
    rcRect.Width = rcBound.Width;
    rcRect.Height = rcBound.Height;
    graph.DrawRectangle(&rectPen, rcRect);
#endif // DRAW_TEXT_RECT

    Gdiplus::REAL lastX = rcLayout.X;
    Gdiplus::REAL lastY = rcLayout.Y + rcBound.Height;

    // Noon: AM or PM
#if USE_CHINESE_TEXT
    if (sysTime.wHour < 12)
        strText = L"上午";
    else
        strText = L"下午";
#else
    if (sysTime.wHour < 12)
        strText = L"AM";
    else
        strText = L"PM";
#endif // USE_CHINESE_TEXT

    // Bottom alignment
	strFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
    Gdiplus::RectF rcBoundNoon;
    graph.MeasureString(strText.GetString(), strText.GetLength(), &fntNoon, GetRectTopLeft(rcLayout), &strFormat, &rcBoundNoon);

    static const Gdiplus::REAL fBottomPadding = 0.28f;
    rcLayout.X += rcBound.Width + 0.0f;
    rcLayout.Y += (rcBound.Height - rcBoundNoon.Height) * (1.0f - fBottomPadding);
    
    graph.MeasureString(strText.GetString(), strText.GetLength(), &fntNoon, GetRectTopLeft(rcLayout), &strFormat, &rcBound);
    //graph.DrawString(strText.GetString(), strText.GetLength(), &fntNoon, rcLayout, &strFormat, &solidBrush);

    // Draw the shadow text
    DrawShadowText(graph, strText, fntNoon, rcLayout, strFormat, solidBrush, shadowBrush, 1.0f, 1.5f);

#if DRAW_TEXT_RECT
    // Draw noon rectangle
    rcRect.X = rcLayout.X;
    rcRect.Y = rcLayout.Y;
    rcRect.Width = rcBound.Width;
    rcRect.Height = rcBound.Height;
    graph.DrawRectangle(&rectPen, rcRect);
#endif // DRAW_TEXT_RECT

#if USE_CHINESE_TEXT
    rcLayout.X = lastX + 18.0f;
#else
    rcLayout.X = lastX + 20.0f;
#endif
    rcLayout.Y = lastY + 8.0f;

    // DayOfWeek: Monday
    strText = GetDayOfWeekString(sysTime.wDayOfWeek);

    // Recover to top alignment
	strFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);

    graph.MeasureString(strText.GetString(), strText.GetLength(), &fntWeek, GetRectTopLeft(rcLayout), &strFormat, &rcBound);
    //graph.DrawString(strText.GetString(), strText.GetLength(), &fntWeek, rcLayout, &strFormat, &solidBrush);

    // Draw the shadow text
    DrawShadowText(graph, strText, fntWeek, rcLayout, strFormat, solidBrush, shadowBrush, 1.0f, 1.2f);

#if DRAW_TEXT_RECT
    // Draw day of week rectangle
    rcRect.X = rcLayout.X;
    rcRect.Y = rcLayout.Y;
    rcRect.Width = rcBound.Width;
    rcRect.Height = rcBound.Height;
    graph.DrawRectangle(&rectPen, rcRect);
#endif // DRAW_TEXT_RECT

    rcLayout.Y += rcBound.Height + 21.0f;

    // Date [Month, Day]: September 18
#if USE_CHINESE_TEXT
    strText.Format(L"%u 月 %u 日", (UINT)sysTime.wMonth, (UINT)sysTime.wDay);
#else
    strText.Format(L"%s %u", GetMonthString(sysTime.wMonth), (UINT)sysTime.wDay);
#endif

    graph.MeasureString(strText.GetString(), strText.GetLength(), &fntDate, GetRectTopLeft(rcLayout), &strFormat, &rcBound);
    //graph.DrawString(strText.GetString(), strText.GetLength(), &fntDate, rcLayout, &strFormat, &solidBrush);

    // Draw the shadow text
    DrawShadowText(graph, strText, fntDate, rcLayout, strFormat, solidBrush, shadowBrush, 1.0f, 1.2f);

#if DRAW_TEXT_RECT
    // Draw date rectangle
    rcRect.X = rcLayout.X;
    rcRect.Y = rcLayout.Y;
    rcRect.Width = rcBound.Width;
    rcRect.Height = rcBound.Height;
    graph.DrawRectangle(&rectPen, rcRect);
#endif // DRAW_TEXT_RECT

#endif // DRAW_CLOCK_TEXT

    // Record last date-time
    RecordLastUpdateTime(sysTime);

    CPoint ptDest(0, 0);
    CSize szDest(rcClient.Width(), rcClient.Height());

    BLENDFUNCTION blendFunc32bpp;
    blendFunc32bpp.AlphaFormat = AC_SRC_ALPHA;
    blendFunc32bpp.BlendFlags = 0;
    blendFunc32bpp.BlendOp = AC_SRC_OVER;
    blendFunc32bpp.SourceConstantAlpha = 255;

    ::UpdateLayeredWindow(m_hWnd, hDC, NULL, &szDest, hMemDC, &ptDest, 0, &blendFunc32bpp, ULW_ALPHA);

    ::SelectObject(hMemDC, hOldBitmap);
    ::DeleteObject(hBitmap);
    ::DeleteDC(hMemDC);
}

void DesktopClockWnd::UpdateLayerWindow()
{
    if (m_hWnd != NULL) {
        HDC hDC = ::GetDC(m_hWnd);
        if (hDC != NULL) {
            // Update date-time info now!
            UpdateLayerWindow(hDC);
        }
        ::ReleaseDC(m_hWnd, hDC);
    }
}
