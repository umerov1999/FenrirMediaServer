#pragma once

#include <memory>
#include <vector>
extern "C"
{
#include "minimp3.h"
}
#include "WaveBlobs.h"

/// MPEG Layer-III decoder
class clMP3DataProvider {
public:
	explicit clMP3DataProvider( const std::shared_ptr<clBlob>& Data );
	virtual ~clMP3DataProvider();

	const sWaveDataFormat& GetWaveDataFormat() const { return m_Format; }

	const uint8_t* GetWaveData() const;
	size_t GetWaveDataSize() const;

	size_t StreamWaveData( size_t Size );
	bool IsStreaming() const { return true; }
	bool IsEndOfStream() const { return m_IsEndOfStream; }
	void Seek( float Seconds );

private:
	int DecodeFromFile( size_t BytesRead );
	void LoadMP3Info();

private:
	std::shared_ptr<clBlob> m_Data;
	sWaveDataFormat m_Format;

	std::vector<uint8_t> m_DecodingBuffer;
	size_t m_BufferUsed;

	size_t m_StreamPos;
	size_t m_InitialStreamPos;
	bool m_IsEndOfStream;

	// minimp3 stuff
	struct DecoderData* m_DecoderData;
};

std::shared_ptr<clMP3DataProvider> CreateMp3WaveDataProvider(const void* Buf, int Size);