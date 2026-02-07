#pragma once

#include "resource.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "afxdialogex.h"
#include "afxeditbrowsectrl.h"

class ExchengeTokenToolDialog : public CDialogEx
{
public:
	ExchengeTokenToolDialog(CWnd* pParent = NULL);
	~ExchengeTokenToolDialog();

	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

public:
	HICON m_hIcon;

	CEdit AccessToken;
	CEdit ExchengeToken;

	CEdit DeviceName;
	CEdit DeviceId;

	CButton UseDeviceName;
	CButton UseDeviceId;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();

	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();

	afx_msg void OnToken2Exchenge();
	afx_msg void OnExchenge2Token();
	afx_msg void OnCheckBox();

	DECLARE_MESSAGE_MAP()
};
