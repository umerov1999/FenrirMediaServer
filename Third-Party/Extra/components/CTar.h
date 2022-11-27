#pragma once
#include <iostream>
#include <cstring>
#include <cstdio>
#include <list>

namespace CTar
{
#define TARBLTYPE_NORMAL '0'
#define TARBLTYPE_DIR '5'
#define TARBLTYPE_LONGFN 'L'
#define ASCII_TO_NUMBER(num) ((num)-48)
#define ctar_min(a,b)(((a) < (b)) ? (a) : (b))
#define ctar_max(a,b)(((a) > (b)) ? (a) : (b))

	struct TARFileHeader {
		char filename[100];
		char mode[8];
		char uid[8];
		char gid[8];
		char fileSize[12];
		char lastModification[12];
		char checksum[8];
		char typeFlag;
		char linkedFileName[100];
		char ustarIndicator[6];
		char ustarVersion[2];
		char ownerUserName[32];
		char ownerGroupName[32];
		char deviceMajorNumber[8];
		char deviceMinorNumber[8];
		char padding[167];
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
		virtual void fopen(const std::wstring &FilePath, const std::wstring &Mode) = 0;
		virtual void fopen(const void*buf, size_t Size) = 0;
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
		virtual void fopen(const std::wstring &FilePath, const std::wstring &Mode)
		{
			if (_wfopen_s(&FilePointer, FilePath.c_str(), Mode.c_str()) != 0)
				return;
			Opened = true;
		}
		virtual void fopen(const void*buf, size_t Size)
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
		virtual void fopen(const std::wstring &FilePath, const std::wstring &Mode)
		{
			return;
		}
		virtual void fopen(const void*buf, size_t Size)
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
			return ctar_min(Size, Count);
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
		size_t FileSize;
		size_t Offset;
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

	static bool CreateFileWraperFromFile(FileWraper*& File, const std::wstring &FilePath, const std::wstring &Mode)
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

	static FileWraper* CreateFileWraperFromMemory(const void*buf, size_t Size)
	{
		FileMemory* file = new FileMemory();
		file->fopen(buf, Size);
		return file;
	}

	struct FilesExist
	{
		FilesExist(const std::wstring &FileName)
		{
			TFileName = FileName;
			TDublicates = 0;
		}
		std::wstring TFileName;
		int TDublicates;
	};

	class TarCreaterWriter
	{
	public:
		TarCreaterWriter(const void* Tar, TARFileHeader TarHdr, const std::wstring &FileName, long long HeaderPos);
		~TarCreaterWriter()
		{
			Finalize();
		}
		void Finalize();

		TarCreaterWriter& Write(const void* Data, size_t Size);
		TarCreaterWriter& operator<<(const std::string &Data);
		TarCreaterWriter& operator<<(const std::wstring &Data);
		TarCreaterWriter& operator<<(int value);
		TarCreaterWriter& AddUTF8Header();

		const std::wstring &GetFileName() const
		{
			return TFileName;
		}
	private:
		TARFileHeader PTarHdr;
		void *CTar;
		std::wstring TFileName;
		long long PosHeader;
		unsigned long long WritedBytes;
	};

	class TarCreater
	{
	public:
		TarCreater()
		{
			fTar = NULL;
			Busy = false;
			PCreateTime = -1;
			TFileName.clear();
			NeedFlush = false;
		}
		TarCreater(const std::wstring &FileName, time_t CreateTime, const std::wstring &Comment = L"", bool AutoFlush = false);
		~TarCreater()
		{
			close();
		}
		void close();
		bool open(const std::wstring &FileName, time_t CreateTime, const std::wstring &Comment = L"", bool Exception = false, bool AutoFlush = false);
		bool is_open() const
		{
			return fTar != NULL;
		}
		const std::wstring &filename() const
		{
			return TFileName;
		}
		void SetBusy(bool bBusy)
		{
			Busy = bBusy;
		}
		void WriteRawData(const void*RawData, int Size)
		{
			if (fTar == NULL)
				return;
			if (fTar->fwrite(RawData, 1, Size) != Size)
				DoTarCreaterException(L"Запись данных невозможна");
		}
		void FlushBytes()
		{
			if (NeedFlush == true)
				fTar->fflush();
		}
		FileWraper* GetFileHandle() const
		{
			return fTar;
		}
		TarCreaterWriter AddFileEntry(const std::wstring &FileName, time_t ModificationTime);
		TarCreater& AddDirectory(const std::wstring &Dir, time_t ModificationTime, bool NoCheckSubDirs = false);
		void CheckAndMakeLongName(TARFileHeader&TarHeader, const std::wstring &FileName, bool isDir);
	private:
		bool NeedFlush;
		bool Busy;
		FileWraper*fTar;
		time_t PCreateTime;
		std::wstring TFileName;
		std::list<std::wstring>Directories;
		std::list<FilesExist>Files;

		void AddComment(const std::wstring &Comment, time_t ModificationTime);
		auto SearchFile(const std::wstring &FileName);
		void CheckAndMakeDirecories(const std::wstring &PathIgnoreFile, time_t ModificationTime);
		bool IsExistDirectory(const std::wstring &PathIgnoreFile);
		void DoTarCreaterException(const std::wstring &Error);
		bool NeedLongHeader(const std::wstring &FileName, int& BlockReserved);
	};

	class TarErrorException : public std::runtime_error
	{
	public:
		TarErrorException(const std::wstring &Error) : runtime_error("Tar Error")
		{
			TError = Error;
		}
		const std::wstring &operator()() const
		{
			return TError;
		}
	private:
		std::wstring TError;
	};

	class TarEntries
	{
	public:
		TarEntries(const std::wstring &FileName, long long Offset, long long FileSize, time_t TimeCreate, FileWraper* ftar)
		{
			TFileName = FileName;
			TOffset = Offset;
			TFileSize = FileSize;
			TTimeCreate = TimeCreate;
			fTar = ftar;
		}
		std::wstring TFileName;
		long long TOffset;
		long long TFileSize;
		time_t TTimeCreate;
		FileWraper* get_file() const
		{
			long long TarSize = fTar->fsize();
			if (TarSize < (TOffset + TFileSize))
				throw TarErrorException(L"TarReader Error: File Offset+Size > TarSize");
			fTar->fseek(TOffset, SEEK_SET);
			return fTar;
		}
	private:
		FileWraper*fTar;
	};

	class TarReader
	{
	public:
		TarReader()
		{
			fTar = NULL;
		}
		~TarReader()
		{
			close();
		}
		std::wstring open(const std::wstring &FileName)
		{
			if (is_open() == true)
				close();
			TarFileName = FileName;
			FileWraper* wrp = NULL;
			if (CreateFileWraperFromFile(wrp, FileName, L"rb") == false)
				return MakeTarReaderError(L"Невозможно открыть файл");
			return Prepare(wrp, FileName);
		}
		std::wstring open(const void* Buf, size_t Size, const std::wstring &FileName)
		{
			TarFileName = FileName;
			return Prepare(CreateFileWraperFromMemory(Buf, Size), FileName);
		}
		auto TarEntryByPath(const std::wstring &Path)
		{
			for (auto i = TTarEntries.begin(); i != TTarEntries.end(); i++)
			{
				if (Path == (*i).TFileName)
					return i;
			}
			throw TTarEntries.end();
		}
		auto TarEntryByIndex(size_t index)
		{
			size_t in = 0;
			for (auto i = TTarEntries.begin(); i != TTarEntries.end(); i++, in++)
			{
				if (in == index)
					return i;
			}
			throw TTarEntries.end();
		}
		void close()
		{
			if (is_open() == false)
				return;
			TTarEntries.clear();
			TarFileName.clear();
			TTarDirectories.clear();
			CloseFileWraper(fTar);
		}
		bool is_open() const
		{
			return fTar != NULL;
		}
		std::list<TarEntries> TTarEntries;
		std::list<std::wstring> TTarDirectories;
	private:
		std::wstring Prepare(FileWraper *Tar, const std::wstring &FileName);
		FileWraper*fTar;
		std::wstring TarFileName;
		std::wstring MakeTarReaderError(const std::wstring &Error);
		uint64_t decodeTarOctal(const char* data, size_t size = 12);
		bool checkChecksum(TARFileHeader &block);
		bool IsZeroBlock(const void*Data, size_t Size)
		{
			for (size_t i = 0; i < Size; i++)
			{
				if (((char*)Data)[i] != 0)
					return false;
			}
			return true;
		}
	};
}