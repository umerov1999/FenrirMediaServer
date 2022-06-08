// XTPSkinManagerResource.h: interface for the CXTPSkinManagerResourceFile class.
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

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPSKINMANAGERRESOURCE_H__)
#	define __XTPSKINMANAGERRESOURCE_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

#include <vector>
#include "TMap.h"
#include "Lz4Pack.h"

struct MainThemesParams
{
	MainThemesParams()
	{
		IsSelected = false;
	}
	ToolkitProMap::Map<ToolkitProMap::string_compare_no_case, MainThemesParams> Themes;
	ToolkitProMap::Map<ToolkitProMap::string_compare_no_case, ToolkitProMap::Map<ToolkitProMap::string_compare_no_case, std::string>> Classes;
	ToolkitProMap::Map<ToolkitProMap::string_compare_no_case, std::string> Macros;
	bool IsSelected;
};

class BUILT_THEME
{
public:
	BUILT_THEME()
	{
		SetSucc(false);
	}
	BUILT_THEME(bool)
	{
		SetSucc(false);
	}
	std::string ToIniThemeUTF8()
	{
		std::string ret;
		if (Classes.size() > 0)
		{
			ret += "\r\n";
			for (auto& i : Classes)
			{
				ret += (i.get_key()() + "\r\n");
				for (auto& prm : i.get_value())
					ret += (prm.get_key()() + " = " + prm.get_value() + "\r\n");
				ret += "\r\n";
			}
		}
		return ret;
	}
	void SetSucc(bool Succ)
	{
		bSucc = Succ;
	}
	bool GetSucc() const
	{
		return bSucc;
	}
	ToolkitProMap::Map<ToolkitProMap::string_compare_no_case, ToolkitProMap::Map<ToolkitProMap::string_compare_no_case, std::string>>& GetClasses()
	{
		return Classes;
	}
private:
	ToolkitProMap::Map<ToolkitProMap::string_compare_no_case, ToolkitProMap::Map<ToolkitProMap::string_compare_no_case, std::string>> Classes;
	bool bSucc;
};

struct SchemaInfo
{
	SchemaInfo()
	{
		Selected = false;
	}
	SchemaInfo(bool Selected, std::string Name)
	{
		this->Name = Name;
		this->Selected = Selected;
	}
	std::string Name;
	bool Selected;
};

class ThemeLoader
{
public:
	BUILT_THEME Build(std::string Scheme = "")
	{
		if (Scheme.length() <= 0)
			Scheme = DefaultSheme;
		BUILT_THEME Ret;
		ToolkitProMap::Map<ToolkitProMap::string_compare_no_case, std::string> Macros = Themes.Macros;
		Ret.GetClasses() = Themes.Classes;

		MainThemesParams* ThParams = &Themes;
		while (Scheme.length() > 0)
		{
			size_t Pos = Scheme.find('.');
			std::string ThemeP = Scheme.substr(0, Pos);
			if (Pos == std::string::npos)
				Scheme.clear();
			else
				Scheme = Scheme.substr(Pos + 1);
			try
			{
				ThParams = &(*ThParams).Themes.at(ThemeP);
				for (auto& i : (*ThParams).Classes)
				{
					for (auto& s : i.get_value())
						Ret.GetClasses()[i.get_key()][s.get_key()] = s.get_value();
				}
				for (auto& i : (*ThParams).Macros)
					Macros[i.get_key()] = i.get_value();
			}
			catch (...)
			{
				return false;
			}
		}
		for (auto& i : Ret.GetClasses())
		{
			for (auto& s : i.get_value())
			{
				bool HasMacr = false;
				for (auto& macr : Macros)
				{
					size_t fnd = std::string::npos;
					while ((fnd = s.get_value().find(macr.get_key()())) != std::string::npos)
					{
						s.get_value().replace(fnd, macr.get_key()().length(), macr.get_value());
						if (HasMacr == false)
							HasMacr = true;
					}
				}
				if (HasMacr == true)
				{
					size_t fnd = std::string::npos;
					while ((fnd = s.get_value().find("\\\\")) != std::string::npos)
						s.get_value().replace(fnd, 2, "\\");
				}

			}
		}
		Ret.SetSucc(true);
		return Ret;
	}
	std::vector<std::string>ListShemes()
	{
		std::vector<std::string>Ret;
		for (auto& i : ShemesInfo)
			Ret.push_back(i.Name);
		return Ret;
	}
	std::string GetDefaultSheme() const
	{
		return DefaultSheme;
	}
	std::string& GetDefaultShemeUnConst()
	{
		return DefaultSheme;
	}
	MainThemesParams& GetThemesMap()
	{
		return Themes;
	}
	std::vector<SchemaInfo>& GetShemesInfoVector()
	{
		return ShemesInfo;
	}
private:
	std::vector<SchemaInfo> ShemesInfo;
	std::string DefaultSheme;
	MainThemesParams Themes;
};

class SkinThemeBuilderFromPack
{
public:
	SkinThemeBuilderFromPack()
	{
	}
	SkinThemeBuilderFromPack(void*)
	{
	}
	SkinThemeBuilderFromPack(const SkinThemeBuilderFromPack& v)
	{
		Theme = v.Theme;
		Files = v.Files;
		ThemeIni = v.ThemeIni;
	}
	void operator=(const SkinThemeBuilderFromPack& v)
	{
		Files.clean();
		Theme = v.Theme;
		Files = v.Files;
		ThemeIni = v.ThemeIni;
	}
	CString ThemeIni;
	ThemeLoader Theme;
	Lz4Pack::Decompressor Files;
};


class CXTPSkinImage;
class CXTPSkinManagerSchema;
class CXTPSkinManager;

//===========================================================================
// Summary:
//     CXTPSkinManagerResourceFile is a standalone class that represents a
//     manager of visual style resources
//===========================================================================
class _XTP_EXT_CLASS CXTPSkinManagerResourceFile
	: public CXTPCmdTarget
	, public CXTPSynchronized
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPSkinManagerResourceFile object.
	// Parameters:
	//     pSkinManager - Pointer to the parent SkinFramework manager.
	//-----------------------------------------------------------------------
	CXTPSkinManagerResourceFile(CXTPSkinManager* pSkinManager = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPSkinManagerResourceFile object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPSkinManagerResourceFile();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Opens the visual style file.
	// Parameters:
	//     lpszResourcePath - Path to the specified visual style.
	//     lpszIniFileName  - Name of the .ini file to load.
	//-----------------------------------------------------------------------
	virtual BOOL Open(const void* pSkinLibrary, int Size, LPCTSTR lpszIniFileName, bool needClean);

	//-----------------------------------------------------------------------
	// Summary:
	//     Reads the .ini file.
	// Parameters:
	//     str - Reference to the string used to load the next line
	//           from the resource file.
	//-----------------------------------------------------------------------
	virtual BOOL ReadString(CString& str);

	//-----------------------------------------------------------------------
	// Summary:
	//     Closes the visual style file.
	//-----------------------------------------------------------------------
	virtual void Close();

	//-----------------------------------------------------------------------
	// Summary:
	//     Loads a specified image from a resource file.
	// Parameters:
	//     strImageFile - Path to the specified image.
	// Returns:
	//     A pointer to the newly loaded CXTPSkinImage object containing
	//     the loaded bitmap.
	//-----------------------------------------------------------------------
	virtual CXTPSkinImage* LoadImage(CString strImageFile);

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates the CXTPSkinManagerSchema class associated with loaded resources.
	// Returns:
	//     A pointer to the newly created CXTPSkinManagerSchema class that
	//     will be used to read resources.
	// See Also:
	//     CXTPSkinManagerSchema
	//-----------------------------------------------------------------------
	virtual CXTPSkinManagerSchema* CreateSchema();

	virtual std::vector<CString> ListShemes();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the .ini file name.
	// Returns:
	//     The .ini file name.
	//-----------------------------------------------------------------------
	CString GetIniFileName() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Gets the owner skin manager object.
	// Returns:
	//     A pointer to the owner skin manager object.
	//-----------------------------------------------------------------------
	CXTPSkinManager* GetSkinManager() const;

protected:
	static bool PrepareThemePack(const void* pSkinLibrary, int Size, SkinThemeBuilderFromPack* m_hModule, bool needClean);
	LPWSTR m_lpTextFile;	// Pointer to the next string.
	LPWSTR m_lpTextFileEnd; // Pointer to end of the file.
	SkinThemeBuilderFromPack m_hModule;              // Handle of the visual style

	CString m_strIniFileName;	 // .ini file name.
	CXTPSkinManager* m_pManager; // Parent manager class.

	friend class CXTPSkinManager;
};

AFX_INLINE CString CXTPSkinManagerResourceFile::GetIniFileName() const
{
	return m_strIniFileName;
}
AFX_INLINE CXTPSkinManager* CXTPSkinManagerResourceFile::GetSkinManager() const
{
	return m_pManager;
}

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPSKINMANAGERRESOURCE_H__)
