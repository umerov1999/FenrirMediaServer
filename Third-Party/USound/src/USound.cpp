#include <iostream>
#include <vector>
#include <Windows.h>
#include "USound.h"
#include "WAVDataEncoder.h"
#include "OGGDataProvider.h"
#include "MP3DataProvider.h"
#pragma comment(lib, "winmm.lib")
using namespace std;

void USound::RegisterResourceMP3Sounds(const string& data)
{
	Stop();
	if (data.size() <= 0)
	{
		return;
	}
	const std::shared_ptr<clMP3DataProvider>& det = CreateMp3WaveDataProvider(data.data(), (int)data.size());
	if (det == NULL)
	{
		return;
	}
	clWAVDataEncoder Enc;
	Enc.ResetEncoder(det->GetWaveDataFormat().m_NumChannels, det->GetWaveDataFormat().m_SamplesPerSecond, det->GetWaveDataFormat().m_BitsPerSample, 1.0f);
	while (det->StreamWaveData(65535))
		Enc.EncodePCMData(det->GetWaveData(), det->GetWaveDataSize());
	MemoryTrack = Enc.FinalizeAndGetMemory();
}

void USound::RegisterResourceOGGSounds(size_t count, HMODULE hModule, LPCWSTR ResType, ...)
{
	va_list args;
	va_start(args, ResType);
	for (size_t i = 0; i < count; i++)
	{
		int ResId = va_arg(args, int);
		HRSRC hResBin = FindResourceW(hModule, MAKEINTRESOURCEW(ResId), ResType);
		if (!hResBin)
			continue;
		HGLOBAL mem_block = LoadResource(hModule, hResBin);
		if(!mem_block)
			continue;
		void* pointer = LockResource(mem_block);
		size_t Size = (int)SizeofResource(hModule, hResBin);

		if (Size <= 0 || mem_block == NULL)
		{
			if (mem_block != NULL)
				FreeResource(mem_block);
			continue;
		}
		const std::shared_ptr<clOGGDataProvider> &det = CreateOggWaveDataProvider(pointer, (int)Size);
		if (det == NULL)
		{
			FreeResource(mem_block);
			continue;
		}
		clWAVDataEncoder Enc;
		Enc.ResetEncoder(det->GetWaveDataFormat().m_NumChannels, det->GetWaveDataFormat().m_SamplesPerSecond, det->GetWaveDataFormat().m_BitsPerSample, 1.0f);
		while (det->StreamWaveData(65535))
			Enc.EncodePCMData(det->GetWaveData(), det->GetWaveDataSize());
		ResourceSounds[ResId] = Enc.FinalizeAndGetResource();
		FreeResource(mem_block);
	}
	va_end(args);
}

void USound::PlayResourceSound(int Res, bool Loop, bool NoStop) const
{
	if (ResourceSounds.exist(Res) == false)
		return;
	DWORD Flags = SND_MEMORY | SND_ASYNC;
	if (Loop)
		Flags |= SND_LOOP;
	if(NoStop)
		Flags |= SND_NOSTOP;

	PlaySoundW((LPCWSTR)ResourceSounds[Res], NULL, Flags);
}

void USound::PlayMemorySound(bool Loop, bool NoStop) const
{
	if (MemoryTrack.size() <= 0)
		return;
	DWORD Flags = SND_MEMORY | SND_ASYNC;
	if (Loop)
		Flags |= SND_LOOP;
	if (NoStop)
		Flags |= SND_NOSTOP;

	PlaySoundW((LPCWSTR)MemoryTrack.data(), NULL, Flags);
}

USound::~USound()
{
	Stop();
	for (auto& i : ResourceSounds)
		free(i.get_value());
	ResourceSounds.clear();
}

void USound::Stop()
{
	PlaySoundW(NULL, NULL, 0);
}