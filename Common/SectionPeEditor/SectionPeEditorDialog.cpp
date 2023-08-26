#include "pch.h"
#include "SectionPeEditorDialog.h"
#include "SectionPeEditorSectionsInfoDialog.h"
#include "SectionPeEditorSectionSetNameDialog.h"
#include <fstream>
#include <pe_bliss.h>
#include <pe_bliss_resources.h>
#include "Shlobj.h"
#include "Hex2BinDialog.h"
#include "pe_structures.h"
#pragma comment (lib, "psapi.lib")

using namespace pe_bliss;
using namespace pe_win;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef WS_EX_LAYERED 
#define WS_EX_LAYERED 0x00080000 
#define LWA_COLORKEY 0x00000001 
#define LWA_ALPHA 0x00000002 
#endif

std::vector<uint32_t> Characters;
int SelIndex = -1;
CComboBox SectionsLst;
CButton Check1;
CButton Check2;
CButton Check3;
CButton Check4;
CButton Check5;

CButton Check6;
CButton Check7;
CButton Check8;
CButton Check9;
CButton Check10;
CButton Check11;

wchar_t* GetStringDate(time_t time, bool Endl)
{
	wchar_t Week[7][4] = { L"Вс", L"Пн", L"Вт", L"Ср", L"Чт", L"Пт", L"Сб" };
	wchar_t Month[12][4] = { L"Янв", L"Фев", L"Мар", L"Апр", L"Май", L"Июн", L"Июл", L"Авг", L"Сен", L"Окт", L"Ноя", L"Дек" };
	wchar_t* Strbuf = new wchar_t[26];

	time_t timet = time;
	tm t_m;
	memset(&t_m, 0, sizeof(tm));
	localtime_s(&t_m, &timet);

	memset(Strbuf, 0, 26 * sizeof(wchar_t));
	swprintf_s(Strbuf, 26, L"%s %s %d %d:%d:%d %d%s", Week[t_m.tm_wday], Month[t_m.tm_mon], t_m.tm_mday, t_m.tm_hour, t_m.tm_min, t_m.tm_sec, 1900 + t_m.tm_year, Endl == true ? L"\n" : L"");
	return Strbuf;
}

static char *GetANSIFromUTF16(const wchar_t * pUTF16)
{
	const long nCharacters = pUTF16 ? (long)wcslen(pUTF16) : 0;
	int nANSICharacters = (sizeof(wchar_t) * nCharacters);
	char * pANSI = new char[nANSICharacters + 1];
	memset(pANSI, 0, (nANSICharacters + 1) * sizeof(char));
	if (pUTF16)
#ifdef _WIN32
		WideCharToMultiByte(CP_ACP, 0, pUTF16, -1, pANSI, nANSICharacters, NULL, NULL);
#else
	{
		setlocale(LC_CTYPE, "");
		wcstombs(pANSI, pUTF16, nANSICharacters);
	}
#endif

	return (char *)pANSI;
}

static wchar_t *GetUTF16FromANSI(const char * pANSI)
{
	const long nCharacters = pANSI ? (long)strlen(pANSI) : 0;
	wchar_t * pUTF16 = new wchar_t[nCharacters + 1];
	memset(pUTF16, 0, sizeof(wchar_t) * (nCharacters + 1));
	if (pANSI)
#ifdef _WIN32
		MultiByteToWideChar(CP_ACP, 0, pANSI, -1, pUTF16, nCharacters);
#else
	{
		setlocale(LC_CTYPE, "");
		mbstowcs(pUTF16, pANSI, nCharacters);
	}
#endif

	return pUTF16;
}

SectionPeEditorDialog::SectionPeEditorDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(SectionPeEditorDialog::IDD, pParent)
{
}

SectionPeEditorDialog::~SectionPeEditorDialog()
{
}

void SectionPeEditorDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, FCEFl);
	DDX_Control(pDX, IDC_EDIT2, FLSeg);
	DDX_Control(pDX, IDC_EDIT3, Platfrm);
	DDX_Control(pDX, IDC_EDIT4, SizeEd);

	DDX_Control(pDX, IDC_CHECK1, Check1);
	DDX_Control(pDX, IDC_CHECK2, Check2);
	DDX_Control(pDX, IDC_CHECK3, Check3);
	DDX_Control(pDX, IDC_CHECK4, Check4);
	DDX_Control(pDX, IDC_CHECK5, Check5);
	DDX_Control(pDX, IDC_CHECK6, Check6);
	DDX_Control(pDX, IDC_CHECK7, Check7);
	DDX_Control(pDX, IDC_CHECK8, Check8);
	DDX_Control(pDX, IDC_CHECK9, Check9);
	DDX_Control(pDX, IDC_CHECK10, Check10);
	DDX_Control(pDX, IDC_CHECK11, Check11);
	DDX_Control(pDX, IDC_DATECL, CLDTEd);

	DDX_Control(pDX, IDC_COMBO1, SectionsLst);
	DDX_Control(pDX, IDC_BUTTON11, pAddDataAfterPe);
	DDX_Control(pDX, IDC_BUTTON12, pHex);
}

BEGIN_MESSAGE_MAP(SectionPeEditorDialog, CDialogEx)

	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SETCURSOR()
	ON_WM_HSCROLL()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, OnReplace)
	ON_BN_CLICKED(IDC_BUTTON2, OnSelect)
	ON_BN_CLICKED(IDC_BUTTON3, OnSecmSelect)
	ON_BN_CLICKED(IDC_BUTTON4, OnAdd)
	ON_BN_CLICKED(IDC_BUTTON5, OnRemove)
	ON_BN_CLICKED(IDC_BUTTON6, OnListSections)
	ON_BN_CLICKED(IDC_BUTTON7, OnExtractSections)
	ON_BN_CLICKED(IDC_BUTTON8, OnRepack)
	ON_BN_CLICKED(IDC_BUTTON9, OnApplyCharSections)
	ON_BN_CLICKED(IDC_BUTTON10, OnExtractOverImage)
	ON_BN_CLICKED(IDC_BUTTON11, OnAddOverImage)
	ON_BN_CLICKED(IDC_BUTTON12, Hex_Section)
	ON_LBN_SELCHANGE(IDC_COMBO1, Select_Section)
END_MESSAGE_MAP()

BOOL SectionPeEditorDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	XTPSkinMgr()->AlphaEnableWindow(m_hWnd, 220);
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	CLDTEd.SetWindowTextW(L"Нет данных");
	Platfrm.SetWindowTextW(L"Нет данных");
	SizeEd.SetWindowTextW(L"Нет данных");
	SectionsLst.SetWindowTextW(L"Нет данных");

	pHex.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON2));
	return TRUE;
}

DWORD WINAPI HexDialogThread(LPVOID Out)
{
	Hex2BinDialog dlgS = Hex2BinDialog();
	dlgS.DoModal();
	return FALSE;
}

static inline bool privCreateThreadDetachedSimpleT(
	LPTHREAD_START_ROUTINE  lpStartAddress,
	const void* lpParameter = NULL, DWORD dwCreationFlags = 0
) {
	DWORD thid = 0;
	HANDLE th = ::CreateThread(NULL, 0, lpStartAddress, (LPVOID)lpParameter, dwCreationFlags, &thid);
	if (!th) {
		return false;
	}
	CloseHandle(th);
	return true;
}

void SectionPeEditorDialog::Hex_Section() {
	privCreateThreadDetachedSimpleT(&HexDialogThread);
}

void SectionPeEditorDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

void SectionPeEditorDialog::OnPaint()
{

	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}

}

HCURSOR SectionPeEditorDialog::OnQueryDragIcon()
{
	return (HCURSOR)m_hIcon;
}

void AddSection(pe_base &image, char* name, const void* mem, long size)
{
	section new_section;

	new_section.set_name(name);

	std::string& raw_data = new_section.get_raw_data();
	raw_data.resize(size);
	memcpy((void*)raw_data.data(), mem, size);
	image.add_section(new_section);
}

void SectionPeEditorDialog::GetPeInfo(const wchar_t* patch)
{
	std::ifstream pe_file(patch, std::ios::in | std::ios::binary);
	if (!pe_file)
	{
		MessageBoxW(L"Невозможно открыть PE файл!", L"Внимание!", MB_ICONINFORMATION);
		return;
	}
	try
	{
		int Selr = SectionsLst.GetCurSel();
		tmpoy = patch;
		CString fgf;
		FCEFl.GetWindowTextW(fgf);
		pe_base image(pe_factory::create_pe(pe_file));
		pe_file.close();

		CLDTEd.SetWindowTextW(GetStringDate(image.get_time_date_stamp(), false));
		image.get_pe_type() == pe_type_32 ? Platfrm.SetWindowTextW(L"32-bit") : Platfrm.SetWindowTextW(L"64-bit");

		TRes = L"Информация о ";
		TRes += fgf.GetString();
		TRes += L":\r\n";
		TRes += L"\r\n[Базовая информация:]\r\n";
		TRes.AppendFormat(L"Entry Point: %u\r\n", image.get_ep());
		TRes.AppendFormat(L"OS Version: %u.%u\r\n", image.get_major_os_version(), image.get_minor_os_version());
		TRes.AppendFormat(L"Subsystem Version: %u.%u\r\n", image.get_major_subsystem_version(), image.get_minor_subsystem_version());
		uint16_t MajorImageVersion, MinorImageVersion, Subsystem;
		uint32_t Win32VersionValue;
		image.get_advanced_pe_version(MajorImageVersion, MinorImageVersion, Win32VersionValue, Subsystem);
		TRes.AppendFormat(L"Subsystem: %u\r\n", Subsystem);
		TRes.AppendFormat(L"Image Version: %u.%u\r\n", MajorImageVersion, MinorImageVersion);
		TRes.AppendFormat(L"Win32VersionValue: %u\r\n", Win32VersionValue);
		TRes.AppendFormat(L"Хешсумма: %u\r\n", image.get_checksum());
		if (image.is_console() || image.is_gui() || image.is_dotnet())
		{
			TRes += L"[Свойства:]\r\n";
			if (image.is_console())
				TRes += (L"Консольное ");
			if (image.is_gui())
				TRes += (L"Диалоговое ");
			if (image.is_dotnet())
				TRes += (L"Net Framework ");
			TRes += L"\r\n";
		}
		
		TRes += L"\r\n[Список секций:]\r\n";
		pe_bliss::section_list &sections = image.get_image_sections();
		SectionsLst.ResetContent();
		Characters.clear();
		for (section_list::const_iterator i = sections.begin(); i < sections.end(); i++)
		{
			TRes += L"Секция: \"";
			TRes += i->get_name().c_str();
			Characters.push_back(i->get_characteristics());
			SectionsLst.AddString(GetUTF16FromANSI(i->get_name().c_str()));
			TRes += L"\"\r\n";

			TRes.AppendFormat(L"Позиция в PE: %u\r\n", i->ImagePosition);
			TRes.AppendFormat(L"Виртуальный адрес: %u\r\n", i->get_virtual_address());
			TRes.AppendFormat(L"Размер: %u\r\n", i->get_size_of_raw_data());
			TRes.AppendFormat(L"Виртуальный размер: %u\r\n", i->get_virtual_size());
			if (!i->empty())
				TRes.AppendFormat(L"Энтропия: %.5lf\r\n", entropy_calculator::calculate_entropy(*i));

			TRes += L"Характеристики:";

			if (i->code() == true)
			{
				TRes += L" Code";
			}
			if (i->initializingdata() == true)
			{
				TRes += L" InitializingData";
			}
			if (i->uninitializingdata() == true)
			{
				TRes += L" UninitializingData";
			}
			if (i->extended_relocation() == true)
			{
				TRes += L" ExtendedRelocation";
			}
			if (i->discardable() == true)
			{
				TRes += L" Discardable";
			}
			if (i->not_cached() == true)
			{
				TRes += L" NotCached";
			}
			if (i->not_paged() == true)
			{
				TRes += L" NotPaged";
			}
			if (i->shared() == true)
			{
				TRes += L" Shared";
			}
			if (i->executable() == true)
			{
				TRes += L" Execute";
			}
			if (i->readable() == true)
			{
				TRes += L" Read";
			}
			if (i->writeable() == true)
			{
				TRes += L" Write";
			}

			TRes += L"\r\n";
			TRes += L"\r\n";
		}

		if (image.has_imports())
		{
			TRes += L"\r\n[Импортируемые библиотеки]\r\n";
			const imported_functions_list imports = get_imported_functions(image);
			for (imported_functions_list::const_iterator it = imports.begin(); it != imports.end(); ++it)
			{
				const import_library& lib = *it;
				TRes.AppendFormat(L"%s\r\n", GetUTF16FromANSI(lib.get_name().c_str()));
			}
			TRes += L"\r\n[Таблица импорта:]";
			for (imported_functions_list::const_iterator it = imports.begin(); it != imports.end(); ++it)
			{
				const import_library& lib = *it;
				TRes.AppendFormat(L"\r\nБиблиотека (%s):\r\n", GetUTF16FromANSI(lib.get_name().c_str()));

				const import_library::imported_list& functions = lib.get_imported_functions();
				for (import_library::imported_list::const_iterator func_it = functions.begin(); func_it != functions.end(); ++func_it)
				{
					const imported_function& func = *func_it;
					TRes += L"[+] ";
					if (func.has_name())
						TRes.AppendFormat(L"%s\r\n", GetUTF16FromANSI(func.get_name().c_str()));
					else
						TRes.AppendFormat(L"#%u\r\n", func.get_ordinal());
				}
			}
		}

		if (image.has_exports())
		{
			TRes += L"\r\n[Таблица экспорта:]";
			export_info info;
			const exported_functions_list exports = get_exported_functions(image, info);
			TRes.AppendFormat(L"\r\n(%s):\r\n", GetUTF16FromANSI(info.get_name().c_str()));
			for (exported_functions_list::const_iterator it = exports.begin(); it != exports.end(); ++it)
			{
				const exported_function& func = *it;
				TRes += L"[+] ";
				if (func.has_name())
					TRes.AppendFormat(L"%s", GetUTF16FromANSI(func.get_name().c_str()));
				else
					TRes.AppendFormat(L"#%u", func.get_ordinal());

				if (func.is_forwarded())
					TRes.AppendFormat(L" -> %s", GetUTF16FromANSI(func.get_forwarded_name().c_str()));
				TRes += L"\r\n";
			}
		}

		long imgsz = (long)image.get_image_sections()[image.get_number_of_sections() - 1].ImagePosition + image.get_image_sections()[image.get_number_of_sections() - 1].get_size_of_raw_data();
		
		wchar_t itl[100];
		_ltow_s(imgsz, itl, 10);
		SizeEd.SetWindowTextW(itl);
		if (Selr == -1)
		{
			SectionsLst.SetCurSel(0);
		}
		else
		{
			SectionsLst.SetCurSel(Selr);
		}
		Select_Section();

		image.get_image_sections().clear();
	}
	catch (const pe_exception& e)
	{
		MessageBoxA(0, e.what(), "Error:", MB_ICONERROR);
		pe_file.close();
		return;
	}
}

void SectionPeEditorDialog::Execute(int ID)
{
	if(MessageBoxW(L"Продолжить?", L"Предупреждение!", MB_ICONINFORMATION | MB_YESNO) == IDNO)
	{
		return;
	}
	CString fgf;
	CString sgmed;
	FCEFl.GetWindowTextW(fgf);
	SectionsLst.GetWindowTextW(sgmed);
	
	CString FLSTRSegment;
	FLSeg.GetWindowTextW(FLSTRSegment);

	if (fgf.GetLength() <= 0)
	{
		MessageBoxW(L"Выберите PE файл!", L"Внимание!", MB_ICONINFORMATION);
		return;
	}

	if (FLSTRSegment.GetLength() <= 0 && ID != 2 && ID != -1 && ID != -2)
	{
		MessageBoxW(L"Выберите файл секции!", L"Внимание!", MB_ICONINFORMATION);
		return;
	}

	if (ID == 1)
	{
		SectionPeEditorSectionSetNameDialog dfgjk;
		dfgjk.DoModal();
		if (dfgjk.Ret == false)
		{
			return;
		}
		else
		{
			sgmed = dfgjk.NmStr;
		}
	}

	if (sgmed.GetLength() <= 0 && ID != -1 && ID != -2)
	{
		MessageBoxW(L"Введите имя секции!", L"Внимание!", MB_ICONINFORMATION);
		return;
	}

	FILE*sgt = 0;
	if (ID != -1 && ID != -2)
	{
		sgt = _wfopen(FLSTRSegment.GetString(), L"rb");
		if (!sgt && ID != 2)
		{
			MessageBoxW(L"Невозможно открыть файл секции!", L"Внимание!", MB_ICONINFORMATION);
			return;
		}
	}

	std::ifstream pe_file(fgf.GetString(), std::ios::in | std::ios::binary);
	if (!pe_file)
	{
		MessageBoxW(L"Невозможно открыть PE файл!", L"Внимание!", MB_ICONINFORMATION);
		return;
	}
	try
	{
		pe_base image(pe_factory::create_pe(pe_file));
		pe_file.close();

		char* segmnm = GetANSIFromUTF16(sgmed.GetString());
		bool Finded = image.SectionExist(segmnm);
		if (Finded == false && (ID == 0 || ID == 2 || ID == -2) && ID != -1)
		{
			MessageBoxW(L"Секция не найдена", L"Внимание!", MB_ICONINFORMATION);
			image.get_image_sections().clear();
			return;
		}
		if (Finded == true && ID == 1 && ID != -1 && ID != -2)
		{
			MessageBoxW(L"Секция уже существует", L"Внимание!", MB_ICONINFORMATION);
			image.get_image_sections().clear();
			return;
		}

		char *Section = 0;

		long szz = 0;
		if (sgt)
		{
			fseek(sgt, 0, SEEK_END);
			szz = ftell(sgt);
			fseek(sgt, 0, SEEK_SET);
			Section = new char[szz];
			fread(Section, sizeof(char), szz, sgt);

			fclose(sgt);
		}
		else if (ID != 2 && ID != -1 && ID != -2)
		{
			image.get_image_sections().clear();
			return;
		}
		//Set Section Atr
		if (ID == -2)
		{
			section& us = image.find_section_by_name(segmnm);
			if (Check1.GetCheck() > 0)
			{
				us.readable(true);
			}
			else
			{
				us.readable(false);
			}
			if (Check2.GetCheck() > 0)
			{
				us.writeable(true);
			}
			else
			{
				us.writeable(false);
			}
			if (Check3.GetCheck() > 0)
			{
				us.executable(true);
			}
			else
			{
				us.executable(false);
			}
			if (Check4.GetCheck() > 0)
			{
				us.initializingdata(true);
			}
			else
			{
				us.initializingdata(false);
			}
			if (Check5.GetCheck() > 0)
			{
				us.code(true);
			}
			else
			{
				us.code(false);
			}
			if (Check6.GetCheck() > 0)
			{
				us.shared(true);
			}
			else
			{
				us.shared(false);
			}
			if (Check7.GetCheck() > 0)
			{
				us.not_paged(true);
			}
			else
			{
				us.not_paged(false);
			}
			if (Check8.GetCheck() > 0)
			{
				us.uninitializingdata(true);
			}
			else
			{
				us.uninitializingdata(false);
			}
			if (Check9.GetCheck() > 0)
			{
				us.not_cached(true);
			}
			else
			{
				us.not_cached(false);
			}
			if (Check10.GetCheck() > 0)
			{
				us.discardable(true);
			}
			else
			{
				us.discardable(false);
			}
			if (Check11.GetCheck() > 0)
			{
				us.extended_relocation(true);
			}
			else
			{
				us.extended_relocation(false);
			}
		}

		if (ID == 1)
		{
			AddSection(image, segmnm, Section, szz);
		}
		else if (ID == 0)
		{
			section &sect = image.find_section_by_name(segmnm);
			std::string &raw_data = sect.get_raw_data();
			if (szz > (int)raw_data.length())
			{
				MessageBoxW(L"Файл секции больше, чем секция в PE файле, усечение размера!", L"Предупреждение!", MB_ICONINFORMATION);
				szz = (int)raw_data.length();
			}
			memset((void*)raw_data.data(), 0, raw_data.length());
			memcpy((void*)raw_data.data(), Section, szz);
		}
		else if(ID != -1 && ID != -2)
		{
			pe_bliss::section_list rtt = image.get_image_sections();
			image.get_image_sections().clear();
			for (auto it = rtt.begin(); it != rtt.end(); it++)
			{
				if (strcmp(it->get_name().data(), segmnm) != 0)
				{
					image.set_section_virtual_size(image.add_section(*it), (*it).get_virtual_size());
				}
			}
			rtt.clear();
		}
		if (ID != -1 && ID != -2)
		{
			free(Section);
		}

		std::ofstream new_pe_file0(fgf.GetString(), std::ios::out | std::ios::binary | std::ios::trunc);
		if (!new_pe_file0)
		{
			image.get_image_sections().clear();
			MessageBoxW(L"Невозможно создать файл в этой папке!", L"Ошибка!", MB_ICONERROR);
			return;
		}
		//Пересобираем PE-файл
		rebuild_pe(image, new_pe_file0);
		new_pe_file0.close();
		image.get_image_sections().clear();
	}
	catch (const pe_exception& e)
	{
		MessageBoxA(0, e.what(), "Error:", MB_ICONERROR);
		pe_file.close();
		if (sgt)
		{
			fclose(sgt);
		}
		return;
	}
	GetPeInfo((wchar_t*)tmpoy.GetString());
	MessageBoxW(L"Выполнено!", L"Внимание!", MB_ICONINFORMATION);
}

void SectionPeEditorDialog::OnSelect()
{
#if (WINVER >= 0x0400)
	DWORD dwFlags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;
#else
	DWORD dwFlags = OFN_OVERWRITEPROMPT;
#endif
	CFileDialog fileDialog(TRUE, NULL, NULL, dwFlags, L"PE File|*.exe|");
	int result = (int)fileDialog.DoModal();
	if (result != 1)
	{
		return;
	}
	else
	{
		FILE*tmp = _wfopen(fileDialog.GetPathName().GetString(), L"rb");

		if (!tmp)
		{
			MessageBoxW(L"Невозможно открыть файл!", L"Внимание!", MB_ICONINFORMATION);
			return;
		}
		fclose(tmp);
		GetPeInfo((wchar_t*)fileDialog.GetPathName().GetString());
		FCEFl.SetWindowTextW(fileDialog.GetPathName().GetString());
	}
}

void SectionPeEditorDialog::OnSecmSelect()
{
#if (WINVER >= 0x0400)
	DWORD dwFlags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;
#else
	DWORD dwFlags = OFN_OVERWRITEPROMPT;
#endif
	CFileDialog fileDialog(TRUE, NULL, NULL, dwFlags, L"Section|*.*|");
	int result = (int)fileDialog.DoModal();
	if (result != 1)
	{
		return;
	}
	else
	{
		FILE*tmp = _wfopen(fileDialog.GetPathName().GetString(), L"rb");
		if (!tmp)
		{
			MessageBoxW(L"Невозможно открыть файл!", L"Внимание!", MB_ICONINFORMATION);
			return;
		}
		fclose(tmp);
		FLSeg.SetWindowTextW(fileDialog.GetPathName().GetString());
	}
}

void SectionPeEditorDialog::OnExtractSections()
{
	CString sect;
	SectionsLst.GetWindowTextW(sect);

	CString fgf;
	FCEFl.GetWindowTextW(fgf);

	if (fgf.GetLength() <= 0)
	{
		MessageBoxW(L"Выберите PE файл!", L"Внимание!", MB_ICONINFORMATION);
		return;
	}
	if (sect.GetLength() <= 0)
	{
		MessageBoxW(L"Выберите секцию!", L"Внимание!", MB_ICONINFORMATION);
		return;
	}
	std::ifstream pe_file(fgf.GetString(), std::ios::in | std::ios::binary);
	if (!pe_file)
	{
		MessageBoxW(L"Невозможно открыть PE файл!", L"Внимание!", MB_ICONINFORMATION);
		return;
	}
	std::string DataSect = "";
	try
	{
		pe_base image(pe_factory::create_pe(pe_file));
		pe_file.close();

		char* segmnm = GetANSIFromUTF16(sect.GetString());
		bool Finded = image.SectionExist(segmnm);
		if (Finded == false)
		{
			MessageBoxW(L"Секция не найдена", L"Внимание!", MB_ICONINFORMATION);
			image.get_image_sections().clear();
			return;
		}

		DataSect = image.find_section_by_name(segmnm).get_raw_data();
		image.get_image_sections().clear();
	}
	catch (const pe_exception& e)
	{
		MessageBoxA(0, e.what(), "Error:", MB_ICONERROR);
		pe_file.close();
		return;
	}
#if (WINVER >= 0x0400)
	DWORD dwFlags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;
#else
	DWORD dwFlags = OFN_OVERWRITEPROMPT;
#endif
	CFileDialog fileDialog(FALSE, L"bin", sect.GetString(), dwFlags, L"Section|*.bin|");
	int result = (int)fileDialog.DoModal();
	if (result != 1)
	{
		return;
	}
	else
	{
		FILE*tmp = _wfopen(fileDialog.GetPathName().GetString(), L"wb");
		if (!tmp)
		{
			MessageBoxW(L"Невозможно записать в файл!", L"Внимание!", MB_ICONINFORMATION);
			return;
		}

		fwrite(DataSect.data(), sizeof(char), DataSect.size(), tmp);
		fclose(tmp);
	}
	MessageBoxW(L"Выполнено!", L"Внимание!", MB_ICONINFORMATION);
}

void SectionPeEditorDialog::OnExtractOverImage()
{
	CString fgf;
	FCEFl.GetWindowTextW(fgf);

	if (fgf.GetLength() <= 0)
	{
		MessageBoxW(L"Выберите PE файл!", L"Внимание!", MB_ICONINFORMATION);
		return;
	}
	long zsz = 0;
	std::ifstream pe_file(fgf.GetString(), std::ios::in | std::ios::binary);
	if (!pe_file)
	{
		MessageBoxW(L"Невозможно открыть PE файл!", L"Внимание!", MB_ICONINFORMATION);
		return;
	}
	try
	{
		pe_base image(pe_factory::create_pe(pe_file));
		pe_file.close();

		zsz = (long)image.get_image_sections()[image.get_number_of_sections() - 1].ImagePosition + image.get_image_sections()[image.get_number_of_sections() - 1].get_size_of_raw_data();

		image.get_image_sections().clear();
	}
	catch (const pe_exception& e)
	{
		MessageBoxA(0, e.what(), "Error:", MB_ICONERROR);
		pe_file.close();
		return;
	}
	FILE*flt = _wfopen(fgf.GetString(), L"rb");
	fseek(flt, 0, SEEK_END);
	long jz = ftell(flt);
	if (jz <= zsz)
	{
		fclose(flt);
		MessageBoxW(L"Данных, после образа не найдено!", L"Внимание!", MB_ICONINFORMATION);
		return;
	}
	fseek(flt, zsz, SEEK_SET);
	char* mbuf = new char[jz - zsz];
	fread(mbuf, 1, jz - zsz, flt);
	fclose(flt);

#if (WINVER >= 0x0400)
	DWORD dwFlags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;
#else
	DWORD dwFlags = OFN_OVERWRITEPROMPT;
#endif
	CFileDialog fileDialog(FALSE, L"bin", L"BinData", dwFlags, L"Bin|*.bin|");
	int result = (int)fileDialog.DoModal();
	if (result != 1)
	{
		free(mbuf);
		return;
	}
	else
	{
		FILE*tmp = _wfopen(fileDialog.GetPathName().GetString(), L"wb");
		if (!tmp)
		{
			free(mbuf);
			MessageBoxW(L"Невозможно записать в файл!", L"Внимание!", MB_ICONINFORMATION);
			return;
		}

		fwrite(mbuf, sizeof(char), jz - zsz, tmp);
		free(mbuf);
		fclose(tmp);
	}
	MessageBoxW(L"Выполнено!", L"Внимание!", MB_ICONINFORMATION);
}

void SectionPeEditorDialog::OnAddOverImage()
{
	CString fgf;
	FCEFl.GetWindowTextW(fgf);

	if (fgf.GetLength() <= 0)
	{
		MessageBoxW(L"Выберите PE файл!", L"Внимание!", MB_ICONINFORMATION);
		return;
	}
#if (WINVER >= 0x0400)
	DWORD dwFlags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;
#else
	DWORD dwFlags = OFN_OVERWRITEPROMPT;
#endif
	CFileDialog fileDialog(TRUE, NULL, NULL, dwFlags, L"BinData|*.*|");
	int result = (int)fileDialog.DoModal();
	if (result != 1)
	{
		return;
	}
	else
	{
		FILE*tmp = _wfopen(fileDialog.GetPathName().GetString(), L"rb");
		if (!tmp)
		{
			MessageBoxW(L"Невозможно прочесть файл!", L"Внимание!", MB_ICONINFORMATION);
			return;
		}
		fseek(tmp, 0, SEEK_END);
		long syto = ftell(tmp);
		fseek(tmp, 0, SEEK_SET);
		char*flt = new char[syto];
		fread(flt, 1, syto, tmp);
		fclose(tmp);

		FILE*tmpy = _wfopen(fgf.GetString(), L"ab");
		if (!tmpy)
		{
			MessageBoxW(L"Невозможно записать в PE Файл!", L"Внимание!", MB_ICONINFORMATION);
			return;
		}
		fwrite(flt, 1, syto, tmpy);
		fclose(tmpy);
	}
	MessageBoxW(L"Выполнено!", L"Внимание!", MB_ICONINFORMATION);
}

void SectionPeEditorDialog::OnReplace()
{
	Execute(0);
}

void SectionPeEditorDialog::OnAdd()
{
	Execute(1);
}

void SectionPeEditorDialog::OnRemove()
{
	Execute(2);
}

void SectionPeEditorDialog::OnListSections()
{
	SectionPeEditorSectionsInfoDialog fghdlg;

	fghdlg.Resu = TRes;
	fghdlg.DoModal();
}

void SectionPeEditorDialog::OnRepack()
{
	Execute(-1);
}

void SectionPeEditorDialog::OnApplyCharSections()
{
	Execute(-2);
}

void SectionPeEditorDialog::Select_Section()
{
	SelIndex = SectionsLst.GetCurSel();
	uint32_t flagsk = Characters[SelIndex];
	if ((flagsk & image_scn_mem_read) != 0)
	{
		Check1.SetCheck(1);
	}
	else
	{
		Check1.SetCheck(0);
	}
	if ((flagsk & image_scn_mem_write) != 0)
	{
		Check2.SetCheck(1);
	}
	else
	{
		Check2.SetCheck(0);
	}
	if ((flagsk & image_scn_mem_execute) != 0)
	{
		Check3.SetCheck(1);
	}
	else
	{
		Check3.SetCheck(0);
	}
	if ((flagsk & image_scn_cnt_initialized_data) != 0)
	{
		Check4.SetCheck(1);
	}
	else
	{
		Check4.SetCheck(0);
	}
	if ((flagsk & image_scn_cnt_code) != 0)
	{
		Check5.SetCheck(1);
	}
	else
	{
		Check5.SetCheck(0);
	}
	if ((flagsk & image_scn_mem_shared) != 0)
	{
		Check6.SetCheck(1);
	}
	else
	{
		Check6.SetCheck(0);
	}
	if ((flagsk & image_scn_mem_not_paged) != 0)
	{
		Check7.SetCheck(1);
	}
	else
	{
		Check7.SetCheck(0);
	}
	if ((flagsk & image_scn_cnt_uninitialized_data) != 0)
	{
		Check8.SetCheck(1);
	}
	else
	{
		Check8.SetCheck(0);
	}
	if ((flagsk & image_scn_mem_not_cached) != 0)
	{
		Check9.SetCheck(1);
	}
	else
	{
		Check9.SetCheck(0);
	}
	if ((flagsk & image_scn_mem_discardable) != 0)
	{
		Check10.SetCheck(1);
	}
	else
	{
		Check10.SetCheck(0);
	}
	if ((flagsk & image_scn_lnk_nreloc_ovfl) != 0)
	{
		Check11.SetCheck(1);
	}
	else
	{
		Check11.SetCheck(0);
	}
}

void SectionPeEditorDialog::OnClose()
{
	XTPSkinMgr()->ExitProgramm();
}

BOOL SectionPeEditorDialog::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->hwnd == FCEFl.GetSafeHwnd() || pMsg->hwnd == FLSeg.GetSafeHwnd() || pMsg->hwnd == pAddDataAfterPe.GetSafeHwnd())
	{
		if (pMsg->message == WM_DROPFILES)
		{
			long ResultQueryFile = DragQueryFileW((HDROP)pMsg->wParam, 0xFFFFFFFF, NULL, 0);
			if (ResultQueryFile == 1)
			{
				ResultQueryFile = DragQueryFileW((HDROP)pMsg->wParam, 0, szBuf, sizeof(szBuf));
				FILE* tmp = _wfopen(szBuf, L"rb");
				if (!tmp)
				{
					MessageBoxW(L"Невозможно открыть файл!", L"Внимание!", MB_ICONINFORMATION);
					return CDialogEx::PreTranslateMessage(pMsg);
				}
				else
				{
					fclose(tmp);
					if (pMsg->hwnd == FLSeg.GetSafeHwnd()) {
						FLSeg.SetWindowTextW(szBuf);
					}
					else if (pMsg->hwnd == FCEFl.GetSafeHwnd()) {
						FCEFl.SetWindowTextW(szBuf);
						GetPeInfo(szBuf);
					}
					else if (pMsg->hwnd == pAddDataAfterPe.GetSafeHwnd()) {
						CString fgf;
						FCEFl.GetWindowTextW(fgf);

						if (fgf.GetLength() <= 0)
						{
							MessageBoxW(L"Выберите PE файл!", L"Внимание!", MB_ICONINFORMATION);
							return CDialogEx::PreTranslateMessage(pMsg);
						}
						FILE* tmp = _wfopen(szBuf, L"rb");
						if (!tmp)
						{
							MessageBoxW(L"Невозможно прочесть файл!", L"Внимание!", MB_ICONINFORMATION);
							return CDialogEx::PreTranslateMessage(pMsg);
						}
						fseek(tmp, 0, SEEK_END);
						long syto = ftell(tmp);
						fseek(tmp, 0, SEEK_SET);
						char* flt = new char[syto];
						fread(flt, 1, syto, tmp);
						fclose(tmp);

						FILE* tmpy = _wfopen(fgf.GetString(), L"ab");
						if (!tmpy)
						{
							MessageBoxW(L"Невозможно записать в PE Файл!", L"Внимание!", MB_ICONINFORMATION);
							return CDialogEx::PreTranslateMessage(pMsg);
						}
						fwrite(flt, 1, syto, tmpy);
						fclose(tmpy);
						MessageBoxW(L"Выполнено!", L"Внимание!", MB_ICONINFORMATION);
					}
				}
			}
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}