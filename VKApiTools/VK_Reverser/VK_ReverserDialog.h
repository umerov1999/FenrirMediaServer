#pragma once

#include "resource.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "afxdialogex.h"
#include "ColorEdit.h"

class VK_ReverserDialog : public CDialogEx
{
public:
	VK_ReverserDialog(CWnd* pParent = NULL);
	~VK_ReverserDialog();

	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

public:
	HICON m_hIcon;
	HCURSOR m_hCursor;

	ColorEdit Edk;
	CButton StartBT;
	CButton ButFolder;
	CEdit CurrentDir;
	CEdit HTTPSPort;
	CEdit SSLPathEd;
	bool IsStart;

	wchar_t szBuf[512];

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnPaint();

	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnStart();
	afx_msg void ListPatch();
	afx_msg void OnSelectSSL();

	afx_msg void OnInitializeCustoms();

	afx_msg void OnClose();

	afx_msg BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
};

