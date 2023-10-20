#pragma once

#include "resource.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "afxdialogex.h"
#include "libimage.h"
#include "XTPComboBoxNotEdited.h"

class VKApiToolsInputLogin : public CDialogEx
{
public:
	VKApiToolsInputLogin(CWnd* pParent = NULL);
	~VKApiToolsInputLogin();

	enum { IDD = IDD_DIALOG3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

public:
	WCHAR szBuf[MAX_PATH];
	HICON m_hIcon;
	CStatic VK_Icon;
	CEdit EdLogin;
	CEdit EdPassword;
	CEdit EdToken;
	CEdit UserName;
	CEdit SMS;
	CXTPComboBoxNotEdited KateMobilePref;
	LIB_IMAGE::win_image VK_IconBitmap;
	void* util_info;

	virtual BOOL OnInitDialog();
	afx_msg BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();

	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void CheckAccess_TOKEN(std::string Token);
	afx_msg void CheckAccess();
	afx_msg void OnLogin();
	afx_msg void OnSelectKateUser();
	afx_msg void OnToken();
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()
};
