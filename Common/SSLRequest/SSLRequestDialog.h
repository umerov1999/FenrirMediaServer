#pragma once

#include "resource.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "afxdialogex.h"
#include "afxeditbrowsectrl.h"

class SSLRequestDialog : public CDialogEx
{
public:
	SSLRequestDialog(CWnd* pParent = NULL);
	~SSLRequestDialog();

	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

public:
	HICON m_hIcon;

	CMFCEditBrowseCtrl File1;
	CMFCEditBrowseCtrl File2;
	CMFCEditBrowseCtrl File3;
	CEdit EPort;
	CButton BStart;
	CEdit EURL;

	wchar_t szBuf[512];

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnStart();

	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()
};
