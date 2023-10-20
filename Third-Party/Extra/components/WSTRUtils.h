#pragma once
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <cstdint>
#define DO_SECOND_OF(first,do_second) if(first) {first = false;} else { do_second; }
namespace WSTRUtils {
	static std::string wchar_to_UTF8(const std::wstring &In) {
		const wchar_t* in = In.c_str();
		std::string out;
		unsigned int codepoint = 0;
		for (in; *in != 0; ++in) {
			if (*in >= 0xd800 && *in <= 0xdbff)
				codepoint = ((*in - 0xd800) << 10) + 0x10000;
			else {
				if (*in >= 0xdc00 && *in <= 0xdfff)
					codepoint |= *in - 0xdc00;
				else
					codepoint = *in;

				if (codepoint <= 0x7f)
					out.append(1, static_cast<char>(codepoint));
				else if (codepoint <= 0x7ff) {
					out.append(1, static_cast<char>(0xc0 | ((codepoint >> 6) & 0x1f)));
					out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
				}
				else if (codepoint <= 0xffff) {
					out.append(1, static_cast<char>(0xe0 | ((codepoint >> 12) & 0x0f)));
					out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
					out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
				}
				else {
					out.append(1, static_cast<char>(0xf0 | ((codepoint >> 18) & 0x07)));
					out.append(1, static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)));
					out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
					out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
				}
				codepoint = 0;
			}
		}
		return out;
	}

	static std::wstring UTF8_to_wchar(const std::string &In) {
		const char* in = In.c_str();
		std::wstring out;
		unsigned int codepoint = 0;
		while (*in != 0) {
			unsigned char ch = static_cast<unsigned char>(*in);
			if (ch <= 0x7f)
				codepoint = ch;
			else if (ch <= 0xbf)
				codepoint = (codepoint << 6) | (ch & 0x3f);
			else if (ch <= 0xdf)
				codepoint = ch & 0x1f;
			else if (ch <= 0xef)
				codepoint = ch & 0x0f;
			else
				codepoint = ch & 0x07;
			++in;
			if (((*in & 0xc0) != 0x80) && (codepoint <= 0x10ffff)) {
				if (sizeof(wchar_t) > 2)
					out.append(1, static_cast<wchar_t>(codepoint));
				else if (codepoint > 0xffff) {
					out.append(1, static_cast<wchar_t>(0xd800 + (codepoint >> 10)));
					out.append(1, static_cast<wchar_t>(0xdc00 + (codepoint & 0x03ff)));
				}
				else if (codepoint < 0xd800 || codepoint >= 0xe000)
					out.append(1, static_cast<wchar_t>(codepoint));
			}
		}
		return out;
	}

	static std::string utf16_to_UTF8(const std::u16string& In) {
		const char16_t* in = In.c_str();
		std::string out;
		unsigned int codepoint = 0;
		for (in; *in != 0; ++in) {
			if (*in >= 0xd800 && *in <= 0xdbff)
				codepoint = ((*in - 0xd800) << 10) + 0x10000;
			else {
				if (*in >= 0xdc00 && *in <= 0xdfff)
					codepoint |= *in - 0xdc00;
				else
					codepoint = *in;

				if (codepoint <= 0x7f)
					out.append(1, static_cast<char>(codepoint));
				else if (codepoint <= 0x7ff) {
					out.append(1, static_cast<char>(0xc0 | ((codepoint >> 6) & 0x1f)));
					out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
				}
				else if (codepoint <= 0xffff) {
					out.append(1, static_cast<char>(0xe0 | ((codepoint >> 12) & 0x0f)));
					out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
					out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
				}
				else {
					out.append(1, static_cast<char>(0xf0 | ((codepoint >> 18) & 0x07)));
					out.append(1, static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)));
					out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
					out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
				}
				codepoint = 0;
			}
		}
		return out;
	}

	static std::u16string UTF8_to_utf16(const std::string& In) {
		const char* in = In.c_str();
		std::u16string out;
		unsigned int codepoint = 0;
		while (*in != 0) {
			unsigned char ch = static_cast<unsigned char>(*in);
			if (ch <= 0x7f)
				codepoint = ch;
			else if (ch <= 0xbf)
				codepoint = (codepoint << 6) | (ch & 0x3f);
			else if (ch <= 0xdf)
				codepoint = ch & 0x1f;
			else if (ch <= 0xef)
				codepoint = ch & 0x0f;
			else
				codepoint = ch & 0x07;
			++in;
			if (((*in & 0xc0) != 0x80) && (codepoint <= 0x10ffff)) {
				if (sizeof(wchar_t) > 2)
					out.append(1, static_cast<wchar_t>(codepoint));
				else if (codepoint > 0xffff) {
					out.append(1, static_cast<wchar_t>(0xd800 + (codepoint >> 10)));
					out.append(1, static_cast<wchar_t>(0xdc00 + (codepoint & 0x03ff)));
				}
				else if (codepoint < 0xd800 || codepoint >= 0xe000)
					out.append(1, static_cast<wchar_t>(codepoint));
			}
		}
		return out;
	}

	static inline char from_hex(char ch) {
		return (char)(isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10);
	}
	static std::string url_decode(const std::string &text) {
		std::ostringstream escaped;
		escaped.fill('0');
		char h = 0;
		for (auto i = text.begin(), n = text.end(); i != n; ++i) {
			std::string::value_type c = (*i);

			if (c == '%') {
				if (i[1] && i[2]) {
					h = from_hex(i[1]) << 4 | from_hex(i[2]);
					escaped << h;
					i += 2;
				}
			}
			else if (c == '+')
				escaped << ' ';
			else
				escaped << c;
		}
		return escaped.str();
	}

	static std::string url_encode(const std::string &value) {
		std::ostringstream escaped;
		escaped.fill('0');
		escaped << std::hex;
		for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
			std::string::value_type cChar = (*i);
			if (((cChar >= '0' && cChar <= '9') || (cChar >= 'A' && cChar <= 'Z') || (cChar >= 'a' && cChar <= 'z')) || cChar == '-' || cChar == '_' || cChar == '.' || cChar == '~') {
				escaped << cChar;
				continue;
			}
			if (cChar == ' ')
			{
				escaped << '+';
				continue;
			}
			escaped << std::uppercase;
			escaped << '%' << std::setw(2) << int((unsigned char)cChar);
			escaped << std::nouppercase;
		}
		return escaped.str();
	}

	static std::wstring ReplaceStringAll(const std::wstring &str, const std::wstring& from, const std::wstring& to) {
		std::wstring ret = str;
		size_t start_pos = 0;
		while ((start_pos = ret.find(from, start_pos)) != std::wstring::npos) {
			ret.replace(start_pos, from.length(), to);
			start_pos += to.length();
		}
		return ret;
	}

	static std::wstring ReplaceSymbsAll(const std::wstring &str, const std::wstring& from, const wchar_t to) {
		std::wstring ret = str;
		for (auto &s : from)
		{
			size_t start_pos = 0;
			while ((start_pos = str.find(s, start_pos)) != std::wstring::npos) {
				ret.replace(start_pos, 1, 1, to);
				start_pos++;
			}
		}
		return ret;
	}

	static std::wstring FixFileName(const std::wstring &Path) {
		std::wstring ret = Path;
		for (auto& i : ret) {
			if(i >= 0x0001U && i < 0x001FU)
				i = L' ';
			else {
				switch (i) {
				case L'\\':
				case L'/':
				case L':':
				case L'*':
				case L'?':
				case L'\"':
				case L'<':
				case L'>':
				case L'|':
					i = L' ';
					break;
				}
			}
		}

		return ret;
	}

	static std::string FixFileName(const std::string &Path) {
		std::string ret = Path;
		for (auto& i : ret) {
			if (i >= 0x01U && i < 0x1FU)
				i = u8' ';
			else {
				switch (i) {
				case u8'\\':
				case u8'/':
				case u8':':
				case u8'*':
				case u8'?':
				case u8'\"':
				case u8'<':
				case u8'>':
				case u8'|':
					i = u8' ';
					break;
				}
			}
		}

		return ret;
	}

	static std::wstring FixFileNameNotSlash(const std::wstring &Path) {
		std::wstring ret = Path;
		for (auto& i : ret) {
			if (i >= 0x0001U && i < 0x001FU)
				i = L' ';
			else {
				switch (i) {
				case L':':
				case L'*':
				case L'?':
				case L'\"':
				case L'<':
				case L'>':
				case L'|':
					i = L' ';
					break;
				}
			}
		}

		return Path;
	}

	static std::string FixFileNameNotSlash(const std::string &Path) {
		std::string ret = Path;
		for (auto& i : ret) {
			if (i >= 0x01U && i < 0x1FU)
				i = u8' ';
			else {
				switch (i) {
				case u8':':
				case u8'*':
				case u8'?':
				case u8'\"':
				case u8'<':
				case u8'>':
				case u8'|':
					i = u8' ';
					break;
				}
			}
		}

		return ret;
	}

	static std::wstring FixFileNameAudio(const std::wstring &Path) {
		std::wstring bad = { L'#', L'%', L'&', L'{', L'}', L'\\', L'<', L'>', L'*', L'?', L'/', L'$', L'\'', L'\"', L':', L'@', L'`', L'|', L'=' };
		std::wstring ret = Path;
		for (auto& i : ret) {
			if (i >= 0x0001U && i < 0x001FU)
				i = L'_';
			else {
				if (bad.find(i) != std::wstring::npos)
					i = L'_';
			}
		}

		return ret;
	}

	static std::string FixFileNameAudio(const std::string &Path) {
		std::string bad = { u8'#', u8'%', u8'&', u8'{', u8'}', u8'\\', u8'<', u8'>', u8'*', u8'?', u8'/', u8'$', u8'\'', u8'\"', u8':', u8'@', u8'`', u8'|', u8'=' };
		std::string ret = Path;
		for (auto& i : ret) {
			if (i >= 0x01U && i < 0x1FU)
				i = u8'_';
			else {
				if (bad.find(i) != std::string::npos)
					i = u8'_';
			}
		}

		return ret;
	}

	static std::string FixJSonSlashs(const std::string& Path) {
		std::string ret = Path;
		for (auto& i : ret) {
			if (i == u8'\\')
				i = u8'/';
		}

		return ret;
	}

	static std::wstring FixJSonSlashs(const std::wstring& Path) {
		std::wstring ret = Path;
		for (auto& i : ret) {
			if (i == L'\\')
				i = L'/';
		}

		return ret;
	}

	static int ParseInteger(std::string str) {
		int ret = 0;
		try {
			ret = std::stoi(str);
		}
		catch (std::invalid_argument e) {
			
		}
		catch (std::out_of_range e) {
			
		}
		return ret;
	}

	static int ParseInteger(std::wstring str) {
		int ret = 0;
		try {
			ret = std::stoi(str);
		}
		catch (std::invalid_argument e) {

		}
		catch (std::out_of_range e) {

		}
		return ret;
	}

	static wchar_t ToWLower(wchar_t c) {
		if (c >= L'А' && c <= L'Я')
			return c + (L'я' > L'Я' ? L'я' - L'Я' : L'Я' - L'я');
		if (c >= L'A' && c <= L'Z')
			return c + (L'z' > L'Z' ? L'z' - L'Z' : L'Z' - L'z');
		return c;
	}

	static std::wstring toLowerW(const std::wstring& str) {
		std::wstring v = str;
		for (auto& i : v) {
			i = ToWLower(i);
		}
		return v;
	}

	static char ToLower(char c) {
		if (c >= u8'A' && c <= u8'Z')
			return c + (u8'z' > u8'Z' ? u8'z' - u8'Z' : u8'Z' - u8'z');
		return c;
	}

	static std::string toLower(const std::string& str) {
		std::string v = str;
		for (auto& i : v) {
			i = ToLower(i);
		}
		return v;
	}

	static bool compare(const std::string &Pattern, const std::string &Cmp) {
		if (Pattern.length() < Cmp.length())
			return false;
		for (size_t i = 0; i < Cmp.length(); i++) {
			if (ToLower(Pattern[i]) != ToLower(Cmp[i]))
				return false;
		}
		return true;
	}

	static bool Wcompare(const std::wstring &Pattern, const std::wstring &Cmp) {
		if (Pattern.length() < Cmp.length())
			return false;
		for (size_t i = 0; i < Cmp.length(); i++) {
			if (ToWLower(Pattern[i]) != ToWLower(Cmp[i]))
				return false;
		}
		return true;
	}

	static size_t wsearch(const std::wstring &Buf, const std::wstring &Find) {
		int dwOffset = 0;
		while (dwOffset + (int)Find.length() <= (int)Buf.length()) {
			if (Wcompare(&Buf[dwOffset], Find) == true)
				return dwOffset;
			dwOffset++;
		}
		return std::string::npos;
	}

	static size_t search(const std::string &Buf, const std::string &Find) {
		int dwOffset = 0;
		while (dwOffset + (int)Find.length() <= (int)Buf.length()) {
			if (compare(&Buf[dwOffset], Find) == true)
				return dwOffset;
			dwOffset++;
		}
		return std::string::npos;
	}

	static size_t not_const_wsearch(const std::wstring &Buf, const std::wstring &Find) {
		int dwOffset = 0;
		while (dwOffset + (int)Find.length() <= (int)Buf.length()) {
			if (Wcompare(&Buf[dwOffset], Find) == true)
				return dwOffset;
			dwOffset++;
		}
		return std::string::npos;
	}

	static size_t not_const_search(const std::string &Buf, const std::string &Find) {
		int dwOffset = 0;
		while (dwOffset + (int)Find.length() <= (int)Buf.length()) {
			if (compare(&Buf[dwOffset], Find) == true)
				return dwOffset;
			dwOffset++;
		}
		return std::string::npos;
	}

	static std::string GetStringDate(const tm* t_m) {
		char Week[7][4] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
		char Month[12][4] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
		char Strbuf[80];

		memset(Strbuf, 0, sizeof(Strbuf));
		sprintf_s(Strbuf, "%s, %d %s %d %d:%d:%d GMT", Week[(*t_m).tm_wday], (*t_m).tm_mday, Month[(*t_m).tm_mon], 1900 + (*t_m).tm_year, (*t_m).tm_hour, (*t_m).tm_min, (*t_m).tm_sec);
		return Strbuf;
	}

	static std::wstring WGetStringDate(const tm& t_m) {
		wchar_t Week[7][4] = { L"Вс", L"Пн", L"Вт", L"Ср", L"Чт", L"Пт", L"Сб" };
		wchar_t Month[12][4] = { L"Янв", L"Фев", L"Мар", L"Апр", L"Май", L"Июн", L"Июл", L"Авг", L"Сен", L"Окт", L"Ноя", L"Дек" };
		wchar_t Strbuf[80];

		memset(Strbuf, 0, sizeof(Strbuf));
		swprintf_s(Strbuf, L"%s. %d %s %d %d.%d.%d", Week[t_m.tm_wday], t_m.tm_mday, Month[t_m.tm_mon], 1900 + t_m.tm_year, t_m.tm_hour, t_m.tm_min, t_m.tm_sec);
		return Strbuf;
	}

	static std::string GetTimeGMT(int SkipDay) {
		tm u;
		const time_t timer = time(NULL);
		gmtime_s(&u, &timer);
		u.tm_mday += SkipDay;
		mktime(&u);
		return GetStringDate(&u);
	}

	static std::wstring GetTimeLocal() {
		tm u;
		const time_t timer = time(NULL);
		localtime_s(&u, &timer);
		return WGetStringDate(u);
	}

	static std::wstring GetTimeAT(time_t timer) {
		tm u;
		localtime_s(&u, &timer);
		return WGetStringDate(u);
	}

	static std::string urlencodewchar(const std::wstring &link) {
		return url_encode(wchar_to_UTF8(link));
	}

	static std::string wchar_to_Cp1251(const std::wstring &In) {
		std::string ret;
		ret.reserve(In.size());
		std::wstring::const_iterator it = In.begin(), it_e = In.end();
		for (; it != it_e; ++it) {
			wchar_t s = *it;
			char c = 0;
			if (s == 0x0451)
				c = char(184);
			else if (s == 0x0401)
				c = char(168);
			else if (s >= 0x350 + 0xc0 && s <= 0x350 + 0xff)
				c = char(s - 0x350);
			else if (s < 0xc0)
				c = char(s);
			else
				c = ' ';
			if (c)
				ret.push_back(c);
		}
		return ret;
	}

	static std::string UTF8_to_Cp1251(const std::string& In) {
		return wchar_to_Cp1251(UTF8_to_wchar(In));
	}

	static std::wstring combine_path(const std::wstring& folder, const std::wstring& child) {
		if (folder.empty()) {
			return child;
		}
		else if (child.empty()) {
			return folder;
		}
		return folder + L"\\" + child;
	}

	static std::wstring combine_root_path(const std::wstring& root, const std::wstring& folder, const std::wstring& child) {
		return combine_path(combine_path(root, folder), child);
	}

	static std::string printBytesCount(int64_t Bytes) {
		int64_t tb = 1099511627776;
		int64_t gb = 1073741824;
		int64_t mb = 1048576;
		int64_t kb = 1024;
		char returnSize[512];

		if (Bytes >= tb)
			sprintf_s(returnSize, (const char*)u8"%.2lf TB", (double)Bytes / tb);
		else if (Bytes >= gb && Bytes < tb)
			sprintf_s(returnSize, (const char*)u8"%.2lf GB", (double)Bytes / gb);
		else if (Bytes >= mb && Bytes < gb)
			sprintf_s(returnSize, (const char*)u8"%.2lf MB", (double)Bytes / mb);
		else if (Bytes >= kb && Bytes < mb)
			sprintf_s(returnSize, (const char*)u8"%.2lf KB", (double)Bytes / kb);
		else if (Bytes < kb)
			sprintf_s(returnSize, (const char*)u8"%.2llu Bytes", Bytes);
		else
			sprintf_s(returnSize, (const char*)u8"%.2llu Bytes", Bytes);
		return returnSize;
	}

	static std::wstring wprintBytesCount(int64_t Bytes) {
		int64_t tb = 1099511627776;
		int64_t gb = 1073741824;
		int64_t mb = 1048576;
		int64_t kb = 1024;
		wchar_t returnSize[512];

		if (Bytes >= tb)
			swprintf_s(returnSize, L"%.2lf TB", (double)Bytes / tb);
		else if (Bytes >= gb && Bytes < tb)
			swprintf_s(returnSize, L"%.2lf GB", (double)Bytes / gb);
		else if (Bytes >= mb && Bytes < gb)
			swprintf_s(returnSize, L"%.2lf MB", (double)Bytes / mb);
		else if (Bytes >= kb && Bytes < mb)
			swprintf_s(returnSize, L"%.2lf KB", (double)Bytes / kb);
		else if (Bytes < kb)
			swprintf_s(returnSize, L"%.2llu Bytes", Bytes);
		else
			swprintf_s(returnSize, L"%.2llu Bytes", Bytes);
		return returnSize;
	}

#ifdef BIND_CONSOLE
	static void BindStdHandlesToConsole() {
		AllocConsole();

		FILE* fl1, *fl2, *fl3;
		_wfreopen_s(&fl1, L"CONIN$", L"r", stdin);
		_wfreopen_s(&fl2, L"CONOUT$", L"w", stderr);
		_wfreopen_s(&fl3, L"CONOUT$", L"w", stdout);

		HANDLE hStdout = CreateFileW(L"CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		HANDLE hStdin = CreateFileW(L"CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		SetStdHandle(STD_OUTPUT_HANDLE, hStdout);
		SetStdHandle(STD_ERROR_HANDLE, hStdout);
		SetStdHandle(STD_INPUT_HANDLE, hStdin);

		std::wclog.clear();
		std::clog.clear();
		std::wcout.clear();
		std::cout.clear();
		std::wcerr.clear();
		std::cerr.clear();
		std::wcin.clear();
		std::cin.clear();
		std::cout << "         Terminal...            " << std::endl << std::endl;
	}
#endif
}