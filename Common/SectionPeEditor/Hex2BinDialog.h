#pragma once

#include "resource.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "afxdialogex.h"

class Hex2BinDialog : public CDialogEx
{
public:
	Hex2BinDialog(CWnd* pParent = NULL);
	~Hex2BinDialog();

	enum { IDD = IDD_DIALOG4 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

public:
	HICON m_hIcon;

	CEdit Edk;
	wchar_t szBuf[512];

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();

	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnClose();

	afx_msg void OnOpen();
	afx_msg void OnCopy();
	afx_msg void OnSave();

	afx_msg void BinToHex(const wchar_t*fl);
	afx_msg void HextToBin(const wchar_t*fl);

	DECLARE_MESSAGE_MAP()
};
