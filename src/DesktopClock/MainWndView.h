/////////////////////////////////////////////////////////////////////////////
//
// View.h : interface of the View class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <atlframe.h>

using namespace ATL;
using namespace WTL;

class MainWndView : public CWindowImpl<MainWndView>
{
public:
	DECLARE_WND_CLASS(NULL)

	BOOL PreTranslateMessage(MSG * pMsg);

	BEGIN_MSG_MAP(MainWndView)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
	END_MSG_MAP()

    // Handler prototypes (uncomment arguments if needed):
    //	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    //	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    //	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};
