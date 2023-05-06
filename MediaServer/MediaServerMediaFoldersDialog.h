#pragma once

#include "resource.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "UListBox.h"
#include "afxdialogex.h"
#include "libimage.h"

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

	LIB_IMAGE::win_image Pdelete;
	LIB_IMAGE::win_image PAdd;

	UListBox ListAudios;
	UListBox ListDiscography;
	UListBox ListVideos;
	UListBox ListPhotoVideos;
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

	static void OnSelectEraseAudios(MediaServerMediaFoldersDialog* pClass, int Idx);
	static void OnSelectEraseVideos(MediaServerMediaFoldersDialog* pClass, int Idx);
	static void OnSelectEraseDiscography(MediaServerMediaFoldersDialog* pClass, int Idx);
	static void OnSelectErasePhotoVideos(MediaServerMediaFoldersDialog* pClass, int Idx);

	DECLARE_MESSAGE_MAP()
};
