#pragma once

#include "resource.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "afxdialogex.h"
#include "ColorEdit.h"

class MediaServerDialog : public CDialogEx
{
public:
	MediaServerDialog(CWnd* pParent = NULL);
	~MediaServerDialog();

	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

public:
	HICON m_hIcon;
	HCURSOR m_hCursor;

	ColorEdit Edk;
	CButton StartBT;
	CButton ButFolder;
	CButton ButSSL;
	CEdit CurrentDir;
	CEdit HTTPSPort;
	CEdit HTTPPort;
	CEdit SSLPathEd;
	CEdit ServerPassword;
	CButton BAutoStart;
	CButton BAudioStop;
	CButton BUseHttps;
	CButton BUseHttp;
	CButton BIsDebug;
	CButton SelectSSL;
	CButton ScanCovers;
	CButton OnlyNews;
	CButton CanEdit;
	CButton PhotosThumb;
	CButton MediaFolders;
	bool IsStart;
	bool IsAutostart;
	bool NeedAutoStart;

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
	afx_msg void OnAutostart();
	afx_msg void OnScanCovers();
	afx_msg void OnMediaFolders();
	afx_msg void OnStopAudio();
	afx_msg void ToggleStopAudio(bool visible);

	afx_msg void OnClose();
	afx_msg void OnProtocol();

	afx_msg BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
};

