#pragma once

#include "resource.h"
#include <stdio.h>
#include "afxdialogex.h"

class SectionPeEditorSectionSetNameDialog : public CDialogEx
{
public:
	SectionPeEditorSectionSetNameDialog(CWnd* pParent = NULL);
	~SectionPeEditorSectionSetNameDialog();

	enum { IDD = IDD_DIALOG3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

public:
	HICON m_hIcon;
	CEdit NameEd;

	CString NmStr;
	bool Ret;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();

	afx_msg void OnOkT();
	afx_msg void OnClose();

	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()
};
