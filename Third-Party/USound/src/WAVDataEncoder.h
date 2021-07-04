#pragma once
#include <vector>

class clWAVDataEncoder
{
public:
	bool ResetEncoder( int NumChannels, int NumSamplesPerSec, int BitsPerSample, float Quality );
	void EncodePCMData( const void* PCMData, size_t PCMDataSizeBytes );
	LPVOID FinalizeAndGetResource();
	std::vector<uint8_t> FinalizeAndGetMemory();

private:
	void OutValue( uint64_t Value, size_t Size );
	void OutBytes( const void* Bytes, size_t Size );
	void OutString( const char* Str );

private:
	size_t m_DataChunkPos;
	std::vector<uint8_t> m_OutputData;
};
