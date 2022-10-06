#pragma once

#include "resource.h"
#include <stdio.h>
#include "afxdialogex.h"

class SectionPeEditorSectionsInfoDialog : public CDialogEx
{
public:
	SectionPeEditorSectionsInfoDialog(CWnd* pParent = NULL);
	~SectionPeEditorSectionsInfoDialog();

	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

public:
	HICON m_hIcon;
	CEdit ResultsEd;

	CString Resu;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();

	afx_msg void OnClose();
	afx_msg void ToWinBuf();

	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()
};
