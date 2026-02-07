// XTPThemeDLL.cpp: implementation of the CXTPThemeDLL class.
//
// (c)1998-2023 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Common/XTPTypeId.h"
#include "Common/XTPFramework.h"
#include "Common/XTPSystemHelpers.h"
#include "Common/XTPSynchro.h"
#include "Common/XTPApplication.h"
#include "Common/XTPSingleton.h"
#include "Common/XTPVC80Helpers.h"
#include "Common/XTPResourceImage.h"
#include "Common/XTPThemeDLL.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// class: CXTPThemeDLL
//////////////////////////////////////////////////////////////////////

CXTPThemeDLL::CXTPThemeDLL()
{
	Init();
}

CXTPThemeDLL::~CXTPThemeDLL()
{
}

BOOL CXTPThemeDLL::LoadThemeDll(CXTPModuleHandle& hModule, LPCTSTR lpszDLLPath, LPCTSTR lpszDllName)
{
	CString strThemeDll;
	strThemeDll.Format(_T("%s\\%s"), lpszDLLPath, lpszDllName);

	if (!FILEEXISTS_S(strThemeDll))
		return FALSE;

	return hModule.LoadLibrary(strThemeDll);
}

BOOL CXTPThemeDLL::Init(HINSTANCE hAppInstance /*= NULL*/, LPCTSTR lpszDllPath /*= NULL*/)
{
	BOOL bResult = FALSE;

	CString strDllPath;

	// construct DLL path.
	if (lpszDllPath == NULL)
	{
		TCHAR szAppPath[_MAX_PATH];
		if (::GetModuleFileName(hAppInstance, szAppPath, _MAX_PATH) == 0)
		{
			return bResult;
		}

		CString strAppPath(szAppPath);
		int nIndex = strAppPath.ReverseFind(_T('\\'));
		if (nIndex > 0)
		{
			strAppPath = strAppPath.Left(nIndex);
		}
		else
		{
			strAppPath.Empty();
		}

		strDllPath += strAppPath + _T("\\Styles\\");
	}
	else
	{
		strDllPath = lpszDllPath;
	}

	if (!DIRECTORYEXISTS_S(strDllPath))
		return bResult;

	// attempt to load Office style theme DLLs.
	bResult |= LoadThemeDll(m_hOffice2007, strDllPath, _T("Office2007.dll"));
	bResult |= LoadThemeDll(m_hOffice2010, strDllPath, _T("Office2010.dll"));
	bResult |= LoadThemeDll(m_hOffice2013, strDllPath, _T("Office2013.dll"));
	bResult |= LoadThemeDll(m_hOffice2016, strDllPath, _T("Office2016.dll"));

	// attempt to load Visual Studio style theme DLLs.
	bResult |= LoadThemeDll(m_hVisualStudio2012, strDllPath, _T("VisualStudio2012.dll"));
	bResult |= LoadThemeDll(m_hVisualStudio2015, strDllPath, _T("VisualStudio2015.dll"));
	bResult |= LoadThemeDll(m_hVisualStudio2017, strDllPath, _T("VisualStudio2017.dll"));
	bResult |= LoadThemeDll(m_hVisualStudio2019, strDllPath, _T("VisualStudio2019.dll"));
	bResult |= LoadThemeDll(m_hVisualStudio2022, strDllPath, _T("VisualStudio2022.dll"));

	// attempt to load Windows style theme DLLs.
	bResult |= LoadThemeDll(m_hWindows7, strDllPath, _T("Windows7.dll"));
	bResult |= LoadThemeDll(m_hWindows10, strDllPath, _T("Windows10.dll"));

	return bResult;
}

BOOL CXTPThemeDLL::SetHandle(CString strINI)
{
	if (strINI.IsEmpty())
		return FALSE;

	strINI.MakeUpper();

	if (strINI.Find(_T("WINDOWS7")) != -1)
	{
		return XTPResourceImages()->SetHandle(m_hWindows7, strINI);
	}
	if (strINI.Find(_T("WINDOWS10")) != -1)
	{
		return XTPResourceImages()->SetHandle(m_hWindows10, strINI);
	}
	if (strINI.Find(_T("OFFICE2007")) != -1)
	{
		return XTPResourceImages()->SetHandle(m_hOffice2007, strINI);
	}
	if (strINI.Find(_T("OFFICE2010")) != -1)
	{
		return XTPResourceImages()->SetHandle(m_hOffice2010, strINI);
	}
	if (strINI.Find(_T("OFFICE2013")) != -1)
	{
		return XTPResourceImages()->SetHandle(m_hOffice2013, strINI);
	}
	if (strINI.Find(_T("OFFICE2016")) != -1)
	{
		return XTPResourceImages()->SetHandle(m_hOffice2016, strINI);
	}
	if (strINI.Find(_T("VISUALSTUDIO2012")) != -1)
	{
		return XTPResourceImages()->SetHandle(m_hVisualStudio2012, strINI);
	}
	if (strINI.Find(_T("VISUALSTUDIO2015")) != -1)
	{
		return XTPResourceImages()->SetHandle(m_hVisualStudio2015, strINI);
	}
	if (strINI.Find(_T("VISUALSTUDIO2017")) != -1)
	{
		return XTPResourceImages()->SetHandle(m_hVisualStudio2017, strINI);
	}
	if (strINI.Find(_T("VISUALSTUDIO2019")) != -1)
	{
		return XTPResourceImages()->SetHandle(m_hVisualStudio2019, strINI);
	}
	if (strINI.Find(_T("VISUALSTUDIO2022")) != -1)
	{
		return XTPResourceImages()->SetHandle(m_hVisualStudio2022, strINI);
	}

	return FALSE;
}

void CXTPThemeDLL::Reset()
{
	XTPResourceImages()->Reset();
}

//////////////////////////////////////////////////////////////////////
// function: XTPThemeDLL()
//////////////////////////////////////////////////////////////////////

CXTPThemeDLL* AFX_CDECL XTPThemeDLL()
{
	return &CXTPSingleton<CXTPThemeDLL>::Instance();
}
