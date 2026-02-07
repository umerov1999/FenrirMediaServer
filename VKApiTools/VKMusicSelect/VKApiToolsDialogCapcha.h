#pragma once

#include "resource.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "afxdialogex.h"
#include "libimage.h"

class VKApiToolsDialogCapcha : public CDialogEx
{
public:
	VKApiToolsDialogCapcha(CString Link, CString UserAgent, CWnd* pParent = NULL);
	~VKApiToolsDialogCapcha();

	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

public:
	HICON m_hIcon;

	CStatic HBitmapCapcha;
	CEdit LinkEd;
	CEdit CodeEd;
	CString LinkStr;
	CString UserAgentStr;

	CString CapchaCode;
	LIB_IMAGE::win_image CapchaBitmap;
	CButton RunBtn;
	bool NeedCapcha;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();

	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();

	afx_msg void OnRun();

	DECLARE_MESSAGE_MAP()
};
