#pragma once
#include <iostream>
#include <list>
#include <vector>
#include "TMap.h"
#pragma warning(disable : 4505)
#pragma warning(disable : 4458)
#pragma warning(disable : 4100)
namespace Lz4Pack
{
#define FILENAME_LEN 255
#define PACK_HEADER "CJSTYLE "
#define lz4pack_min(a,b)(((a) < (b)) ? (a) : (b))
#define lz4pack_max(a,b)(((a) > (b)) ? (a) : (b))

#pragma pack(push, 1)
	class PackerEntry {
	public:
		PackerEntry() {
			original_size = 0;
			compressed_size = 0;
			memset(file_name, 0, FILENAME_LEN);
			isDir = false;
		}
		int32_t original_size;
		int32_t compressed_size;
		bool isDir;
		char file_name[FILENAME_LEN];
	};
	class PackerHeader {
	public:
		PackerHeader() {
			this->count = -1;
			this->entries_compressed_size = -1;
			memset(hdr, 0, 10);
		}
		PackerHeader(int count, int entries_compressed_size) {
			this->count = count;
			this->entries_compressed_size = entries_compressed_size;
			memset(hdr, 0, sizeof(PACK_HEADER));
			memcpy(hdr, PACK_HEADER, strlen(PACK_HEADER));
		}
		bool is_valid_header(const int buffer_size) {
			return strncmp(hdr, PACK_HEADER, sizeof(hdr)) == 0 && count > 0 && buffer_size > entries_compressed_size;
		}
		static bool check_header_size(int size) {
			if (size <= sizeof(PackerHeader)) {
				return false;
			}
			return true;
		}
		char hdr[10];
		int32_t count;
		int32_t entries_compressed_size;
	};
#pragma pack (pop)

	class DecomressorEntry {
	public:
		DecomressorEntry() {
			originalSize = 0;
			compressedSize = 0;
			offset = 0;
		}
		DecomressorEntry(int originalSize, int compressedSize, int offset) {
			this->originalSize = originalSize;
			this->compressedSize = compressedSize;
			this->offset = offset;
		}
		int originalSize;
		int compressedSize;
		int offset;
	};

	class Decompressor {
	public:
		Decompressor() {
			buf = NULL;
			size = 0;
			needClean = false;
		}
		~Decompressor() {
			if (needClean) {
				delete buf;
				buf = NULL;
			}
		}
		void clean() {
			if (needClean) {
				delete buf;
				buf = NULL;
			}
		}
		Decompressor(const void* buf, int size, bool needClean) {
			this->buf = buf;
			this->size = size;
			this->needClean = needClean;
		}
		bool init(const void* buf, int size, bool needClean) {
			this->buf = buf;
			this->size = size;
			this->needClean = needClean;
			Files.clear();
			Dirs.clear();
			return read_headers();
		}
		bool read_headers();
		std::string operator[](const DecomressorEntry& entry) const;
		std::string operator[](const std::wstring& name_entry) const;
		bool has(const std::wstring& name_entry) const;
		ToolkitProMap::Map<ToolkitProMap::wstring_compare_no_case, DecomressorEntry> Files;
		std::list<std::wstring> Dirs;
	private:
		const void* buf;
		int size;
		bool needClean;
	};

	class FileWraper
	{
	public:
		enum class FILE_TYPE
		{
			FILE_TYPE_FILE,
			FILE_TYPE_MEMORY
		};
		FileWraper()
		{
			Type = FILE_TYPE::FILE_TYPE_MEMORY;
			Opened = false;
		}
		virtual void fopen(const std::wstring& FilePath, const std::wstring& Mode) = 0;
		virtual void fopen(const void* buf, size_t Size) = 0;
		virtual void fseek(long long Offset, int Origin) = 0;
		virtual long long ftell() = 0;
		virtual void fflush() = 0;
		virtual size_t fread(const void* buf, size_t ElemSize, size_t Count) = 0;
		virtual size_t fwrite(const void* buf, size_t ElemSize, size_t Count) = 0;
		virtual long long fsize() = 0;
		bool IsOpen() const
		{
			return Opened;
		}
		FILE_TYPE GetType()
		{
			return Type;
		}
	protected:
		bool Opened;
		FILE_TYPE Type;
	};

	class FileReal :public FileWraper
	{
	public:
		FileReal()
		{
			FileSize = -1;
			FilePointer = NULL;
			Type = FILE_TYPE::FILE_TYPE_FILE;
		}
		~FileReal()
		{
			if (FilePointer == NULL)
				return;
			::fclose(FilePointer);
			FileSize = -1;
			FilePointer = NULL;
			Opened = false;
		}
		virtual void fopen(const std::wstring& FilePath, const std::wstring& Mode)
		{
			if (_wfopen_s(&FilePointer, FilePath.c_str(), Mode.c_str()) != 0)
				return;
			Opened = true;
		}
		virtual void fopen(const void* buf, size_t Size)
		{
		}
		virtual void fseek(long long Offset, int Origin)
		{
			_fseeki64(FilePointer, Offset, Origin);
		}
		virtual long long ftell()
		{
			return _ftelli64(FilePointer);
		}
		virtual void fflush()
		{
			::fflush(FilePointer);
		}
		virtual size_t fread(const void* buf, size_t ElemSize, size_t Count)
		{
			return ::fread((void*)buf, ElemSize, Count, FilePointer);
		}
		virtual size_t fwrite(const void* buf, size_t ElemSize, size_t Count)
		{
			return ::fwrite(buf, ElemSize, Count, FilePointer);
		}
		virtual long long fsize()
		{
			if (FileSize < 0)
			{
				long long TempIU = this->ftell();
				this->fseek(0, SEEK_END);
				FileSize = this->ftell();
				this->fseek(TempIU, SEEK_SET);
			}
			return FileSize;
		}
	private:
		FILE* FilePointer;
		long long FileSize;
	};

	class FileMemory :public FileWraper
	{
	public:
		FileMemory()
		{
			FileSize = -1;
			Offset = -1;
			Buffer = NULL;
			Type = FILE_TYPE::FILE_TYPE_MEMORY;
		}
		~FileMemory()
		{
			FileSize = -1;
			Offset = -1;
			Buffer = NULL;
			Opened = false;
		}
		virtual void fopen(const std::wstring& FilePath, const std::wstring& Mode)
		{
			return;
		}
		virtual void fopen(const void* buf, size_t Size)
		{
			Offset = 0;
			FileSize = Size;
			Buffer = (void*)buf;
			Opened = true;
		}
		virtual void fseek(long long Offset, int Origin)
		{
			if (Origin == SEEK_END)
			{
				if (Offset > (long long)FileSize || Offset < 0)
					return;
				this->Offset = FileSize - (size_t)Offset;
			}
			else if (Origin == SEEK_SET)
			{
				if (Offset > (long long)FileSize || Offset < 0)
					return;
				this->Offset = (size_t)Offset;
			}
			else if (SEEK_CUR)
			{
				long long Temp = this->Offset;
				Temp += Offset;
				if (Temp < 0 || Temp >(long long)FileSize)
					return;
				this->Offset = (size_t)Temp;
			}
		}
		virtual long long ftell()
		{
			return Offset;
		}
		virtual void fflush()
		{

		}
		virtual size_t fread(const void* buf, size_t ElemSize, size_t Count)
		{
			size_t End = FileSize - Offset;
			size_t Size = ElemSize * Count;
			if (Size > End)
				Size = End - (End % ElemSize);

			memcpy((void*)buf, &((char*)Buffer)[Offset], Size);
			Offset += Size;
			return lz4pack_min(Size, Count);
		}
		virtual size_t fwrite(const void* buf, size_t ElemSize, size_t Count)
		{
			return 0;
		}
		virtual long long fsize()
		{
			return FileSize;
		}
	private:
		void* Buffer;
		long long FileSize;
		long long Offset;
	};

	static void CloseRealWraper(FileWraper*& fl)
	{
		FileReal* file = (FileReal*)fl;
		delete file;
	}

	static void CloseMemoryWraper(FileWraper*& fl)
	{
		FileMemory* file = (FileMemory*)fl;
		delete file;
	}

	static void CloseFileWraper(FileWraper*& fl)
	{
		if (fl->GetType() == FileWraper::FILE_TYPE::FILE_TYPE_FILE)
		{
			CloseRealWraper(fl);
			fl = NULL;
		}
		else if (fl->GetType() == FileWraper::FILE_TYPE::FILE_TYPE_MEMORY)
		{
			CloseMemoryWraper(fl);
			fl = NULL;
		}
	}

	static bool CreateFileWraperFromFile(FileWraper*& File, const std::wstring& FilePath, const std::wstring& Mode)
	{
		File = new FileReal();
		File->fopen(FilePath, Mode);
		if (File->IsOpen() == false)
		{
			CloseFileWraper(File);
			return false;
		}
		return true;
	}

	static FileWraper* CreateFileWraperFromMemory(const void* buf, size_t Size)
	{
		FileMemory* file = new FileMemory();
		file->fopen(buf, Size);
		return file;
	}
}