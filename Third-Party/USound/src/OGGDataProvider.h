#pragma once

#include <memory>
#include <vector>

#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>
#include "WaveBlobs.h"

class clOGGDataProvider {
public:
	clOGGDataProvider( const std::shared_ptr<clBlob>& Data );
	~clOGGDataProvider();

	const sWaveDataFormat& GetWaveDataFormat() const { return m_Format; }

	const uint8_t* GetWaveData() const;
	size_t GetWaveDataSize() const;

	size_t StreamWaveData( size_t Size );
	void Seek( float Seconds );

private:
	int DecodeFromFile( size_t Size, size_t BytesRead );

	static size_t oggReadFunc( void* Ptr, size_t Size, size_t NumMemBlocks, void* UserData );
	static int oggSeekFunc( void* UserData, ogg_int64_t Offset, int Whence );
	static int oggCloseFunc( void* UserData );
	static long oggTellFunc( void* UserData );

	std::shared_ptr<clBlob> m_Data;
	sWaveDataFormat m_Format;

	std::vector<uint8_t> m_DecodingBuffer;
	size_t m_BufferUsed;

	bool m_IsEndOfStream;

	// Vorbis stuff
	OggVorbis_File m_VorbisFile;
	ogg_int64_t m_RawPosition;
	int m_CurrentSection;
};

std::shared_ptr<clOGGDataProvider> CreateOggWaveDataProvider(const void* Buf, int Size);