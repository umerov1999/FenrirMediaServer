﻿#pragma once

#include "resource.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "afxdialogex.h"
#include "ColorEdit.h"
#include "UListBox.h"
#include "WSTRUtils.h"
#include "XTPComboBoxNotEdited.h"

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
	UListBox Edk;
	CButton Exec;
	CButton Download;
	CButton Play;
	CButton Stop;
	CEdit NewToken;
	CXTPComboBoxNotEdited FFunc;

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
	afx_msg void OnDownload();
	afx_msg void OnPlay();
	afx_msg void OnStop();
	afx_msg static void OnSelectAudio(VKApiToolsDialog* pClass, int Idx);

	DECLARE_MESSAGE_MAP()
};

struct AudioInfo
{
	AudioInfo(const std::string& tartist, const std::string& ttitle, const std::string& turl, const std::string& tdir, int64_t tdate, bool tDuplicated, int64_t towner_id, const std::string& tsha3_512, const std::string& talbum, const std::string& tcover_url)
	{
		artist = WSTRUtils::FixFileNameAudio(tartist);
		title = WSTRUtils::FixFileNameAudio(ttitle);
		owner_id = towner_id;
		url = turl;
		dir = tdir;
		date = tdate;
		IsError = false;
		Duplicated = tDuplicated;
		sha3_512 = tsha3_512;
		album = WSTRUtils::FixFileNameAudio(talbum);
		cover_url = tcover_url;
	}
	AudioInfo()
	{
		date = 0;
		owner_id = 0;
		IsError = false;
		Duplicated = false;
	}
	std::string artist;
	std::string title;
	std::string album;
	std::string cover_url;
	std::string url;
	std::string dir;
	std::string sha3_512;
	int64_t owner_id;
	int64_t date;
	bool IsError;
	bool Duplicated;
};