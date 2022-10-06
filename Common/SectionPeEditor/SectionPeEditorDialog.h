#pragma once

#include "resource.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>
#include "afxdialogex.h"

class SectionPeEditorDialog : public CDialogEx
{
public:
	SectionPeEditorDialog(CWnd* pParent = NULL);
	~SectionPeEditorDialog();

	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

public:
	HICON m_hIcon;
	CEdit FCEFl;
	CEdit Platfrm;
	CEdit SizeEd;

	CEdit FLSeg;
	CEdit CLDTEd;

	CString TRes;

	CString tmpoy;

	CButton pHex;

	wchar_t szBuf[512];

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();

	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnClose();
	afx_msg void Execute(int ID);
	afx_msg void OnSelect();
	afx_msg void OnSecmSelect();

	afx_msg void OnReplace();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnRepack();
	afx_msg void GetPeInfo(const wchar_t* patch);

	afx_msg void OnListSections();
	afx_msg void OnExtractSections();
	afx_msg void OnApplyCharSections();

	afx_msg void OnExtractOverImage();
	afx_msg void OnAddOverImage();
	afx_msg void Select_Section();
	afx_msg void Hex_Section();

	DECLARE_MESSAGE_MAP()
};
