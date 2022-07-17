#include <Windows.h>
#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include "CTar.h"
#include "WSTRUtils.h"
using namespace std;
using namespace WSTRUtils;
using namespace CTar;

static void UnixTimeToFileTime(const time_t &t, LPFILETIME pft)
{
	LONGLONG ll;
	ll = Int32x32To64(t, 10000000) + 116444736000000000;
	pft->dwLowDateTime = (DWORD)ll;
	pft->dwHighDateTime = ll >> 32;
}

static bool WriteOctal_8(unsigned int val, char *s)
{
	const unsigned kNumDigits = 8 - 1;
	if (val >= ((int)1 << (kNumDigits * 3)))
		return false;
	for (unsigned i = 0; i < kNumDigits; i++)
	{
		s[kNumDigits - 1 - i] = (char)('0' + (val & 7));
		val >>= 3;
	}
	return true;
}

static void WriteOctal_12(unsigned long long val, char *s)
{
	const unsigned kNumDigits = 12 - 1;
	if (val >= ((unsigned long long)1 << (kNumDigits * 3)))
	{
		s[0] = (char)(unsigned char)0x80;
		s[1] = s[2] = s[3] = 0;
		for (unsigned i = 0; i < 8; i++, val <<= 8)
			s[4 + i] = (char)(val >> 56);
		return;
	}
	for (unsigned i = 0; i < kNumDigits; i++)
	{
		s[kNumDigits - 1 - i] = (char)('0' + (val & 7));
		val >>= 3;
	}
}

static void WriteOctal_12_Signed(long long val, char *s)
{
	if (val >= 0)
	{
		WriteOctal_12(val, s);
		return;
	}
	s[0] = s[1] = s[2] = s[3] = (char)(unsigned char)0xFF;
	for (unsigned i = 0; i < 8; i++, val <<= 8)
		s[4 + i] = (char)(val >> 56);
}

static void MakeTarHeader(TARFileHeader& Header, bool Finalize, time_t Modification = -1)
{
	if (Finalize == false)
	{
		memset(&Header, 0, sizeof(Header));
		strcpy_s(Header.ustarIndicator, "ustar");
		memcpy(Header.ustarVersion, "00", 2);
		WriteOctal_12_Signed(Modification, Header.lastModification);
		WriteOctal_8(0, Header.uid);
		WriteOctal_8(0, Header.gid);
		memset(Header.checksum, ' ', sizeof(Header.checksum));
		WriteOctal_12(0, Header.fileSize);
		return;
	}

	unsigned int checkSum = 0;
	{
		for (unsigned i = 0; i < sizeof(TARFileHeader); i++)
			checkSum += ((unsigned char*)&Header)[i];
	}
	const unsigned kNumDigits = 6;
	for (unsigned i = 0; i < kNumDigits; i++)
	{
		((char*)&Header)[148 + kNumDigits - 1 - i] = (char)('0' + (checkSum & 7));
		checkSum >>= 3;
	}
	((char*)&Header)[148 + 6] = 0;
	((char*)&Header)[148 + 7] = ' ';
}

static void FixFilePathSlashes(wstring &FilePath)
{
	for (auto &i : FilePath)
	{
		if (i == L'\\')
			i = L'/';
	}
}

TarCreaterWriter::TarCreaterWriter(const void* Tar, TARFileHeader TarHdr, const wstring &FileName, long long HeaderPos)
{
	CTar = (void*)Tar;
	((TarCreater*)CTar)->SetBusy(true);
	PTarHdr = TarHdr;
	TFileName = FileName;
	PosHeader = HeaderPos;
	WritedBytes = 0;
}

void TarCreaterWriter::Finalize()
{
	if (CTar == NULL)
		return;
	((TarCreater*)CTar)->GetFileHandle()->fseek(PosHeader, SEEK_SET);
	WriteOctal_12(WritedBytes, PTarHdr.fileSize);
	((TarCreater*)CTar)->CheckAndMakeLongName(PTarHdr, TFileName, false);
	MakeTarHeader(PTarHdr, true);
	((TarCreater*)CTar)->WriteRawData(&PTarHdr, sizeof(TARFileHeader));
	((TarCreater*)CTar)->GetFileHandle()->fseek(0, SEEK_END);

	unsigned long long len = WritedBytes;
	size_t FreeBlockSz = 0;
	while ((len++ % sizeof(TARFileHeader)) != 0)
		++FreeBlockSz;
	if (FreeBlockSz > 0)
	{
		vector<char> EmptyBlock(FreeBlockSz);
		memset(EmptyBlock.data(), 0, FreeBlockSz);
		((TarCreater*)CTar)->WriteRawData(EmptyBlock.data(), (int)EmptyBlock.size());
	}
	((TarCreater*)CTar)->FlushBytes();
	((TarCreater*)CTar)->SetBusy(false);
	CTar = NULL;
	PosHeader = -1;
	memset(&PTarHdr, 0, sizeof(TARFileHeader));
	TFileName.clear();
}

TarCreaterWriter& TarCreaterWriter::Write(const void* Data, size_t Size)
{
	((TarCreater*)CTar)->WriteRawData(Data, (int)Size);
	WritedBytes += Size;
	return *this;
}

TarCreaterWriter& TarCreaterWriter::operator<<(const std::string &Data)
{
	((TarCreater*)CTar)->WriteRawData(Data.data(), (int)Data.size());
	WritedBytes += Data.size();
	return *this;
}


TarCreaterWriter& TarCreaterWriter::operator<<(const std::wstring &Data)
{
	std::string ETemp = wchar_to_UTF8(Data);
	((TarCreater*)CTar)->WriteRawData(ETemp.data(), (int)ETemp.size());
	WritedBytes += ETemp.size();
	return *this;
}

TarCreaterWriter& TarCreaterWriter::operator<<(int value)
{
	std::string ETemp = to_string(value);
	((TarCreater*)CTar)->WriteRawData(ETemp.data(), (int)ETemp.size());
	WritedBytes += ETemp.size();
	return *this;
}

TarCreaterWriter& TarCreaterWriter::AddUTF8Header()
{
	((TarCreater*)CTar)->WriteRawData("\xef\xbb\xbf", 3);
	WritedBytes += 3;
	return *this;
}

void TarCreater::DoTarCreaterException(const wstring &Error)
{
	wstring RError = L"TarCreater Error: " + Error + L", File \"" + TFileName + L"\"";
	throw TarErrorException(RError);
}

TarCreater::TarCreater(const wstring &FileName, time_t CreateTime, const wstring &Comment, bool AutoFlush)
{
	fTar = NULL;
	NeedFlush = AutoFlush;
	open(FileName, CreateTime, Comment, true);
}

bool TarCreater::open(const std::wstring &FileName, time_t CreateTime, const wstring &Comment, bool Exception, bool AutoFlush)
{
	if (is_open() == true)
		close();
	PCreateTime = CreateTime;
	TFileName = FileName;
	NeedFlush = AutoFlush;
	Busy = false;
	if (CreateFileWraperFromFile(fTar, FileName, L"wb") == false)
	{
		if (Exception == true)
			DoTarCreaterException(L"Невозможно создать файл");
		else
			return false;
	}
	if (Comment.length() > 0)
		AddComment(Comment, CreateTime);
	return true;
}

void TarCreater::close()
{
	if (is_open() == false)
		return;
	char zeroBlock[sizeof(TARFileHeader)];
	memset(zeroBlock, 0, sizeof(zeroBlock));
	for (unsigned i = 0; i < 2; i++)
		fTar->fwrite(zeroBlock, 1, sizeof(zeroBlock));
	CloseFileWraper(fTar);
	HANDLE file_handle =
		::CreateFileW(TFileName.c_str(), FILE_WRITE_ATTRIBUTES,
			NULL, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, 0);
	if (file_handle != NULL)
	{
		FILETIME FlTime;
		UnixTimeToFileTime(PCreateTime, &FlTime);
		::SetFileTime(file_handle, &FlTime, &FlTime, &FlTime);
		CloseHandle(file_handle);
	}
	TFileName.clear();
	PCreateTime = -1;
	Directories.clear();
	Files.clear();
	NeedFlush = false;
	Busy = false;
}

bool TarCreater::IsExistDirectory(const std::wstring &PathIgnoreFile)
{
	return find(Directories.begin(), Directories.end(), PathIgnoreFile) != Directories.end();
}

void TarCreater::CheckAndMakeDirecories(const std::wstring &PathIgnoreFile, time_t ModificationTime)
{
	wstring PathIgnoreFl = PathIgnoreFile;
	wstring Temp;
	do
	{
		size_t Pos = PathIgnoreFl.find(L'/');
		if (Pos == wstring::npos)
		{
			if (Temp.length() > 0)
				Temp += (L"/" + PathIgnoreFl);
			else
				Temp += PathIgnoreFl;
			PathIgnoreFl.clear();
		}
		else
		{
			if (Temp.length() > 0)
				Temp += (L"/" + PathIgnoreFl.substr(0, Pos));
			else
				Temp = PathIgnoreFl.substr(0, Pos);
			PathIgnoreFl = PathIgnoreFl.substr(Pos + 1);
		}
		if (IsExistDirectory(Temp) == false)
			AddDirectory(Temp, ModificationTime, true);
	} while (!PathIgnoreFl.empty());
}

void TarCreater::CheckAndMakeLongName(TARFileHeader&TarHeader, const wstring &FileName, bool isDir)
{
	string UTF8FileName = wchar_to_UTF8(FileName);
	if (isDir == false && UTF8FileName.length() <= 0 || isDir == true && UTF8FileName.length() <= 1)
		DoTarCreaterException(L"Нулевая длинна файла");
	if(UTF8FileName.length() > sizeof(TARFileHeader::filename))
	{
		TARFileHeader BlockHdr = TarHeader;
		memcpy(BlockHdr.filename, "././@LongLink", 13);
		BlockHdr.typeFlag = TARBLTYPE_LONGFN;
		WriteOctal_12(UTF8FileName.length(), BlockHdr.fileSize);
		MakeTarHeader(BlockHdr, true);
		WriteRawData(&BlockHdr, sizeof(TARFileHeader));
		WriteRawData(UTF8FileName.data(), (int)UTF8FileName.length());
		unsigned long long len = UTF8FileName.length();
		size_t FreeBlockSz = 0;
		while ((len++ % sizeof(TARFileHeader)) != 0)
			++FreeBlockSz;
		if (FreeBlockSz > 0)
		{
			vector<char> EmptyBlock(FreeBlockSz);
			memset(EmptyBlock.data(), 0, FreeBlockSz);
			WriteRawData(EmptyBlock.data(), (int)EmptyBlock.size());
		}
	}
	memcpy(TarHeader.filename, UTF8FileName.data(), min(sizeof(TarHeader.filename), UTF8FileName.length()));
}

bool TarCreater::NeedLongHeader(const std::wstring &FileName, int& BlockReserved)
{
	BlockReserved = 0;
	string UTF8FileName = wchar_to_UTF8(FileName);
	if (UTF8FileName.length() <= 0)
		DoTarCreaterException(L"Нулевая длинна файла");
	if (UTF8FileName.length() > sizeof(TARFileHeader::filename))
	{
		size_t Blocks = UTF8FileName.length();
		while ((Blocks++ % sizeof(TARFileHeader)) != 0);
		Blocks /= sizeof(TARFileHeader);
		BlockReserved = (int)Blocks;
		return true;
	}
	return false;
}

TarCreater& TarCreater::AddDirectory(const wstring &Dir, time_t ModificationTime, bool NoCheckSubDirs)
{
	wstring Dr = Dir;
	if (Busy == true)
		DoTarCreaterException(L"Невозможно создать файл/директорию, пока не запишется текущий файл");
	FixFilePathSlashes(Dr);
	if (IsExistDirectory(Dr) == true)
		return *this;
	if (NoCheckSubDirs == false)
		CheckAndMakeDirecories(Dir, ModificationTime);
	else
	{
		TARFileHeader BlockHdr;
		MakeTarHeader(BlockHdr, false, ModificationTime);
		BlockHdr.typeFlag = TARBLTYPE_DIR;
		strcpy_s(BlockHdr.mode, "0040777");
		Directories.push_back(Dr);
		Dr += L"/";
		CheckAndMakeLongName(BlockHdr, Dr, true);
		MakeTarHeader(BlockHdr, true);
		WriteRawData(&BlockHdr, sizeof(TARFileHeader));
	}
	return *this;
}

auto TarCreater::SearchFile(const wstring &FileName)
{
	for (auto i = Files.begin(); i != Files.end(); i++)
	{
		if ((*i).TFileName == FileName)
		{
			(*i).TDublicates++;
			return i;
		}
	}
	return Files.end();
}

static std::wstring StringSeporator(std::wstring fname, std::wstring Find, bool Second, bool NExEmp)
{
	size_t pos = fname.find_last_of(Find);
	return (std::wstring::npos == pos)
		? (NExEmp == false ? fname : L"")
		: (Second == false ? fname.substr(0, pos) : fname.substr(pos + 1));
}

static wstring MakeFile(wstring Str, bool Path)
{
	if (Str.length() > 0)
	{
		if (Path == true)
			return Str + L"/";
		else
			return L"." + Str;
	}
	return L"";
}

TarCreaterWriter TarCreater::AddFileEntry(const wstring &FileName, time_t ModificationTime)
{
	wstring FlName = FileName;
	if (Busy == true)
		DoTarCreaterException(L"Невозможно создать файл/директорию, пока не запишется текущий файл");
	FixFilePathSlashes(FlName);
	auto fndi = SearchFile(FlName);
	if (fndi != Files.end())
	{
		wstring Path = StringSeporator(FlName, L"\\/", false, true);
		wstring File = StringSeporator(FlName, L"\\/", true, false);
		wstring Ext = StringSeporator(File, L".", true, true);
		File = StringSeporator(File, L".", false, false);
		FlName = MakeFile(Path, true) + File + L" (" + to_wstring((*fndi).TDublicates) + L")" + MakeFile(Ext, false);
	}
	else
		Files.push_back(FilesExist(FlName));
	if (FlName.find_last_of(L'/') != wstring::npos)
		CheckAndMakeDirecories(FlName.substr(0, FlName.find_last_of(L'/')), ModificationTime);
	TARFileHeader BlockHdr;
	MakeTarHeader(BlockHdr, false, ModificationTime);
	strcpy_s(BlockHdr.mode, "0100777");
	BlockHdr.typeFlag = TARBLTYPE_NORMAL;
	char zeroBlock[sizeof(TARFileHeader)];
	memset(zeroBlock, 0, sizeof(zeroBlock));
	long long PosHeader = fTar->ftell();

	int NameBlocks = 0;
	if (NeedLongHeader(FlName, NameBlocks) == true)
	{
		NameBlocks += 2;
		for (int i = 0; i < NameBlocks; i++)
			WriteRawData(zeroBlock, sizeof(zeroBlock));
	}
	else
		WriteRawData(zeroBlock, sizeof(zeroBlock));
	return TarCreaterWriter(this, BlockHdr, FlName, PosHeader);
}

uint64_t TarReader::decodeTarOctal(const char* data, size_t size)
{
	unsigned char* currentPtr = (unsigned char*)data + size;
	uint64_t sum = 0;
	uint64_t currentMultiplier = 1;
	unsigned char* checkPtr = currentPtr;
	for (; checkPtr >= (unsigned char*)data; checkPtr--) {
		if ((*checkPtr) == 0 || (*checkPtr) == ' ') {
			currentPtr = checkPtr - 1;
		}
	}
	for (; currentPtr >= (unsigned char*)data; currentPtr--) {
		sum += ASCII_TO_NUMBER(*currentPtr) * currentMultiplier;
		currentMultiplier *= 8;
	}
	return sum;
}

bool TarReader::checkChecksum(TARFileHeader &block)
{
	char originalChecksum[8];
	memcpy(originalChecksum, block.checksum, 8);
	memset(block.checksum, ' ', 8);
	int64_t unsignedSum = 0;
	int64_t signedSum = 0;
	for (int i = 0; i < sizeof(TARFileHeader); i++) {
		unsignedSum += ((unsigned char*)&block)[i];
		signedSum += ((signed char*)&block)[i];
	}
	memcpy(block.checksum, originalChecksum, 8);
	uint64_t referenceChecksum = decodeTarOctal(originalChecksum);
	return (referenceChecksum == unsignedSum || referenceChecksum == signedSum);
}

std::wstring TarReader::MakeTarReaderError(const std::wstring &Error)
{
	return L"TarReader Error: " + Error + L", File \"" + TarFileName + L"\"";
}

wstring TarReader::Prepare(FileWraper *Tar, const wstring &FileName)
{
	fTar = Tar;
	TARFileHeader block;
	while (true)
	{
		if (fTar->fread(&block, sizeof(TARFileHeader), 1) != 1)
			return MakeTarReaderError(L"Непредвиденный конец блока");
		if (IsZeroBlock(&block, sizeof(TARFileHeader)) == true)
			break;
		if (checkChecksum(block) == false)
			return MakeTarReaderError(L"Tar блок повреждён, Offset " + to_wstring(fTar->ftell()));

		string UTF8FileName;
		if (block.typeFlag == TARBLTYPE_LONGFN)
		{
			int FlNameSz = (int)decodeTarOctal(block.fileSize);
			UTF8FileName.resize(FlNameSz + 1);
			if (fTar->fread(UTF8FileName.data(), 1, FlNameSz) != FlNameSz)
				return MakeTarReaderError(L"Непредвиденный конец блока");
			int StrOff = 0;
			while ((FlNameSz % sizeof(TARFileHeader)) != 0)
			{
				++FlNameSz;
				++StrOff;
			}
			fTar->fseek(StrOff, SEEK_CUR);

			if (fTar->fread(&block, sizeof(TARFileHeader), 1) != 1)
				return MakeTarReaderError(L"Непредвиденный конец блока");
			if (checkChecksum(block) == false)
				return MakeTarReaderError(L"Tar блок повреждён, Offset " + to_wstring(fTar->ftell()));
		}
		else if (block.typeFlag != TARBLTYPE_LONGFN && block.typeFlag != TARBLTYPE_DIR && block.typeFlag != TARBLTYPE_NORMAL)
			return MakeTarReaderError(L"Найден неопознанный блок");
		else
		{
			UTF8FileName.resize(sizeof(block.filename) + 1);
			memcpy((char*)UTF8FileName.data(), block.filename, sizeof(block.filename));
		}
		if (block.typeFlag == TARBLTYPE_DIR)
		{
			TTarDirectories.push_back(UTF8_to_wchar(UTF8FileName));
			continue;
		}
		if (block.typeFlag != TARBLTYPE_NORMAL)
			continue;

		unsigned long long FileSize = decodeTarOctal(block.fileSize);
		TTarEntries.push_back(TarEntries(UTF8_to_wchar(UTF8FileName), fTar->ftell(), FileSize, decodeTarOctal(block.lastModification), fTar));
		while ((FileSize % sizeof(TARFileHeader)) != 0)
			++FileSize;
		fTar->fseek(FileSize, SEEK_CUR);
	}
	return L"";
}

void TarCreater::AddComment(const wstring &Comment, time_t ModificationTime)
{
	if (Comment.length() < 0)
		return;
	TarCreaterWriter CommentData = AddFileEntry(L"Tar Комментарий.txt", ModificationTime);
	CommentData.AddUTF8Header() << Comment;
	CommentData.Finalize();
}