#pragma once

#include "resource.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <stdio.h>
#include "afxdialogex.h"
#include "ColorEdit.h"
#include "Ulottie.h"
#include "XTPComboBoxNotEdited.h"

class Story {
public:
	Story(const std::wstring &Token, const std::wstring &Method, const std::wstring &Params) {
		this->Token = Token;
		this->Method = Method;
		this->Params = Params;
	}
	bool operator==(const Story& o) {
		return o.Token == Token && o.Params == Params && o.Method == Method;
	}
	std::wstring Token;
	std::wstring Method;
	std::wstring Params;
};

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
	Ulottie Anim;
	CButton Exec;
	CEdit NewToken;
	CEdit APIMethod;
	CEdit ParamsEd;
	CXTPComboBoxNotEdited FFunc;

	CButton Left;
	CButton Right;

	CButton Check1;
	CButton Check2;
	CButton Check3;
	CButton Check4;

	int historyPos;
	std::vector<Story> stories;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();

	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	afx_msg void OnExecute();
	afx_msg void OnSelect();
	afx_msg void OnAccount();

	afx_msg void OnLeft();
	afx_msg void OnRight();
	afx_msg void OnGetStory();
	afx_msg void OnInsert(const Story& v);

	afx_msg void checkButtons();

	DECLARE_MESSAGE_MAP()
};
