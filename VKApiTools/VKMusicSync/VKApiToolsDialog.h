#pragma once

#include "resource.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "afxdialogex.h"
#include "ColorEdit.h"

class VKApiToolsDialog : public CDialogEx
{
public:
	VKApiToolsDialog(CWnd* pParent = NULL);
	~VKApiToolsDialog();

	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

public:
	HICON m_hIcon;
	ColorEdit Edk;
	CButton Exec;
	CEdit NewToken;
	CEdit OldToken;
	CComboBox FFunc;

	CButton Check1;
	CButton Check2;
	CButton Check3;
	CButton Check4;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();

	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	afx_msg void OnExecute();
	afx_msg void OnSelect();
	afx_msg void OnAccount();

	DECLARE_MESSAGE_MAP()
};
