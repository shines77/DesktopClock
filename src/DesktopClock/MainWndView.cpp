/////////////////////////////////////////////////////////////////////////////
//
// MainWndView.cpp : implementation of the MainWndView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "MainWndView.h"

BOOL MainWndView::PreTranslateMessage(MSG * pMsg)
{
	UNREFERENCED_PARAMETER(pMsg);

	return FALSE;
}

LRESULT MainWndView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);

	//TODO: Add your drawing code here

	return 0;
}
