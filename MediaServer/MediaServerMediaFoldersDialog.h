#pragma once

#include "resource.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "afxdialogex.h"

class MediaServerMediaFoldersDialog : public CDialogEx
{
public:
	MediaServerMediaFoldersDialog(CWnd* pParent = NULL);
	~MediaServerMediaFoldersDialog();

	enum { IDD = IDD_DIALOG2};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

public:
	HICON m_hIcon;
	HCURSOR m_hCursor;

	CListBox ListAudios;
	CListBox ListDiscography;
	CListBox ListVideos;
	CListBox ListPhotoVideos;
	CButton EraseAudios;
	CButton EraseVideos;
	CButton ErasePhotoVideos;
	CButton EraseDiscography;
	CButton AddAudios;
	CButton AddVideos;
	CButton AddPhotoVideos;
	CButton AddDiscography;

	wchar_t szBuf[512];

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnPaint();

	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();

	afx_msg void OnAddAudio();
	afx_msg void OnDeleteAudio();

	afx_msg void OnAddDiscography();
	afx_msg void OnDeleteDiscography();

	afx_msg void OnAddVideo();
	afx_msg void OnDeleteVideo();

	afx_msg void OnAddPhotoVideo();
	afx_msg void OnDeletePhotoVideo();

	afx_msg void ReloadContent();
	afx_msg BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
};
