#pragma once

#include <memory>
#include <vector>

class clBlob
{
public:
	explicit clBlob(const std::vector<uint8_t>& Data)
		: m_Data(Data)
	{}

	const std::vector<uint8_t>& GetData() const { return m_Data; }
	size_t GetDataSize() const { return m_Data.size(); }
	const uint8_t* GetDataPtr() const { return m_Data.data(); }

	void Clear() { m_Data.clear(); }

private:
	std::vector<uint8_t> m_Data;
};

class sWaveDataFormat
{
public:
	sWaveDataFormat()
		: m_NumChannels(0)
		, m_SamplesPerSecond(0)
		, m_BitsPerSample(0)
	{}

public:
	int m_NumChannels;
	int m_SamplesPerSecond;
	int m_BitsPerSample;
};