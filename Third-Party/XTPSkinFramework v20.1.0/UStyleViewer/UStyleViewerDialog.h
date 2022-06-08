#pragma once

#include "resource.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "afxdialogex.h"

class UStyleViewerDialog : public CDialogEx
{
public:
	UStyleViewerDialog(CWnd* pParent = NULL);
	~UStyleViewerDialog();

	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

public:
	HICON m_hIcon;
	HCURSOR m_hCursor;

	CComboBox wSchms;

	CListBox Lc;
	CComboBox Cmb;
	CEdit Ed;
	CEdit POEd;
	CButton Rd;
	CButton Test;
	CButton TestRd;
	CSliderCtrl Sldr;
	CString Header;

	wchar_t szBuf[512];

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();

	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	afx_msg void OnCloseStyle();
	afx_msg void OnSwitchShem();
	afx_msg void UpdateShems();
	afx_msg void OnBrowseFolder();

	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()
};
