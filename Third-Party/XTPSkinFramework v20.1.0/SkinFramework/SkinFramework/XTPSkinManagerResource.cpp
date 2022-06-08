// XTPSkinManagerResource.cpp: implementation of the CXTPSkinManagerResourceFile class.
//
// (c)1998-2021 Codejock Software, All Rights Reserved.
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
#include "Common/XTPGdiObjects.h"
#include "Common/XTPVC80Helpers.h"
#include "Common/XTPColorManager.h"
#include "Common/XTPSynchro.h"

#include "SkinFramework/XTPSkinManagerResource.h"
#include "SkinFramework/XTPSkinImage.h"
#include "SkinFramework/XTPSkinManager.h"
#include "SkinFramework/XTPSkinManagerSchema.h"

#include <iostream>
#include <vector>
#include "Lz4Pack.h"
#include "Xml/pugixml.hpp"
#include "WSTRUtils.h"
#include "../GraphicLibrary/lz4/lz4.h"

#include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

using namespace Lz4Pack;

#ifdef _DEBUG
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#	define new DEBUG_NEW
#endif

CXTPSkinManagerResourceFile::CXTPSkinManagerResourceFile(CXTPSkinManager* pManager)
{
	m_lpTextFile = m_lpTextFileEnd = NULL;
	m_hModule					   = NULL;
	m_pManager					   = pManager ? pManager : XTPSkinManager();
}

CXTPSkinManagerResourceFile::~CXTPSkinManagerResourceFile()
{
	Close();
}

void CXTPSkinManagerResourceFile::Close()
{
	m_hModule = NULL;
}

CXTPSkinManagerSchema* CXTPSkinManagerResourceFile::CreateSchema()
{
	return new CXTPSkinManagerSchema(this);
}

static std::wstring GetExtInPath(std::wstring fname)
{
	const wchar_t FnS = L'.';
	size_t pos = fname.find_last_of(FnS);
	return (std::wstring::npos == pos)
		? fname
		: fname.substr(pos + 1);
}

static void EnumShemes(MainThemesParams& Theme, std::vector<SchemaInfo>& Info, std::string Last = "")
{
	if (Theme.Themes.size() <= 0)
	{
		Info.push_back(SchemaInfo(Theme.IsSelected, Last));
		return;
	}
	for (auto& i : Theme.Themes)
	{
		std::string Temp = Last;
		if (Last.length() > 0)
			Temp += ".";
		EnumShemes(i.get_value(), Info, Temp + i.get_key()());
	}
}

static void ParseTheme(const pugi::xml_node& node, MainThemesParams& Theme, bool IsSelected)
{
	Theme.IsSelected = IsSelected;
	for (auto& lv_Val : node)
	{
		if (std::string(lv_Val.name()) == "ThemeInfo")
			ParseTheme(lv_Val, Theme.Themes[lv_Val.attribute("Caption").as_string()], lv_Val.attribute("Selected").as_bool());
		else if (std::string(lv_Val.name()) == "Class")
		{
			for (auto& lv_Par : lv_Val)
			{
				if (std::string(lv_Par.name()) == "Property")
				{
					if (std::string(lv_Val.attribute("Name").as_string()) == "Macros")
					{
						Theme.Macros[std::string("%") + lv_Par.attribute("Name").as_string() + "%"] = lv_Par.attribute("Value").as_string();
						continue;
					}
					std::string Part = lv_Par.attribute("Part").as_string();
					std::string Name = std::string("[") + lv_Val.attribute("Name").as_string();
					std::string State = lv_Par.attribute("State").as_string();
					if (Part.length() > 0)
						Name += ("." + Part);
					if (State.length() > 0)
						Name += ("(" + State + ")");
					Name += "]";
					Theme.Classes[Name][lv_Par.attribute("Name").as_string()] = lv_Par.attribute("Value").as_string();
				}
			}
		}
	}
}

static bool LoadThemeXML(std::string& XMLDt, ThemeLoader* ThmLdr)
{
	pugi::xml_document doc;
	doc.load_buffer(XMLDt.data(), XMLDt.size());
	const pugi::xml_node& lv_Node = doc.child("SkinBuilderProject").child("ThemeInfo");
	if (std::string(lv_Node.attribute("Caption").as_string()) != "Common")
		return false;
	ParseTheme(lv_Node, ThmLdr->GetThemesMap(), false);
	EnumShemes(ThmLdr->GetThemesMap(), ThmLdr->GetShemesInfoVector());

	for (auto& i : ThmLdr->GetShemesInfoVector())
	{
		if (i.Selected == true)
		{
			ThmLdr->GetDefaultShemeUnConst() = i.Name;
			break;
		}
	}
	if (ThmLdr->GetDefaultShemeUnConst().length() <= 0)
		return false;
	return true;
}

bool CXTPSkinManagerResourceFile::PrepareThemePack(const void* pSkinLibrary, int Size, SkinThemeBuilderFromPack* m_hModule, bool needClean)
{
	if (Size <= 0)
		return false;
	if (!m_hModule->Files.init(pSkinLibrary, Size, needClean))
		return false;
	std::wstring XMLDataFile;
	for (auto& i : m_hModule->Files.Files)
	{
		std::string Data;
		if (GetExtInPath(i.get_key()()) == L"skinproj") {
			XMLDataFile = i.get_key()();
			break;
		}
	}
	if (XMLDataFile.empty()) {
		return false;
	}

	std::string XMLDt = m_hModule->Files[XMLDataFile];

	return LoadThemeXML(XMLDt, &m_hModule->Theme);
}

BOOL CXTPSkinManagerResourceFile::Open(const void* pSkinLibrary, int Size, LPCTSTR lpszIniFileName, bool needClean)
{
	if (pSkinLibrary != NULL && Size > 0)
	{
		Close();
		if (PrepareThemePack(pSkinLibrary, Size, &m_hModule, needClean) == false)
		{
			m_hModule = NULL;
			return FALSE;
		}
	}

	m_strIniFileName = lpszIniFileName;
	if (m_strIniFileName.IsEmpty())
		m_strIniFileName = ToolkitProWSTRUtils::UTF8_to_wchar(m_hModule.Theme.GetDefaultSheme()).c_str();

	BUILT_THEME ThemeEnd = m_hModule.Theme.Build(ToolkitProWSTRUtils::wchar_to_UTF8(m_strIniFileName.GetString()));
	if (ThemeEnd.GetSucc() == false)
	{
		m_hModule = NULL;
		return FALSE;
	}
	std::wstring tempE = (L"Z" + ToolkitProWSTRUtils::UTF8_to_wchar(ThemeEnd.ToIniThemeUTF8()));
	tempE[0] = 0xFEFF;
	m_hModule.ThemeIni = tempE.c_str();

	m_lpTextFile = (LPWSTR)m_hModule.ThemeIni.GetString();

	m_lpTextFileEnd = m_lpTextFile + m_hModule.ThemeIni.GetLength();

	if (m_lpTextFile && m_lpTextFile[0] == 0xFEFF)
		m_lpTextFile++;

	return TRUE;
}

BOOL CXTPSkinManagerResourceFile::ReadString(CString& str)
{
	ASSERT(m_lpTextFile);
	if (!m_lpTextFile)
		return FALSE;

	LPWSTR pNextLine = m_lpTextFile;

	while ((pNextLine < m_lpTextFileEnd) && (*pNextLine != L'\0') && (*pNextLine != L'\r'))
		pNextLine++;

	if (pNextLine == NULL || pNextLine >= m_lpTextFileEnd || *pNextLine == L'\0')
		return FALSE;

	str = CString(m_lpTextFile, (int)(pNextLine - m_lpTextFile));

	m_lpTextFile = pNextLine + 1;
	if (m_lpTextFile < m_lpTextFileEnd && m_lpTextFile[0] == L'\n')
		m_lpTextFile = m_lpTextFile + 1;

	return TRUE;
}

CXTPSkinImage* CXTPSkinManagerResourceFile::LoadImage(CString strImageFile)
{
	strImageFile.Replace(_T('\\'), _T('/'));
	strImageFile.Replace(_T("//"), _T("/"));
#ifdef _UNICODE
	if (!m_hModule.Files.has(strImageFile.GetString())) {
		return NULL;
	}
	std::string Data = m_hModule.Files[strImageFile.GetString()];
#else
	if (!m_hModule.Files.has(ToolkitProWSTRUtils::UTF8_to_wchar(strImageFile.GetString()))) {
		return NULL;
	}
	std::string Data = m_hModule.Files[ToolkitProWSTRUtils::UTF8_to_wchar(strImageFile.GetString())];
#endif
	CXTPSkinImage* pImage = new CXTPSkinImage();
	if (!pImage->LoadMemory(Data.data(), (int)Data.size()))
	{
		delete pImage;
		return NULL;
	}
	return pImage;
}

AFX_INLINE std::vector<CString> CXTPSkinManagerResourceFile::ListShemes() {
	std::vector<CString> Ret;
	for (auto& i : m_hModule.Theme.ListShemes())
		Ret.push_back(ToolkitProWSTRUtils::UTF8_to_wchar(i).c_str());
	return Ret;
}
