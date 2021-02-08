#pragma once

#include <iostream>
#include "Map.h"
#include <Windows.h>

#define THREAD_ACCESS_REGISTER_POINTERS(guard,...)  guard.Register(THREAD_ACCESS_GUARD::va_count(__VA_ARGS__), __VA_ARGS__)
#define THREAD_ACCESS_LOCK(guard,...) guard.Lock(THREAD_ACCESS_GUARD::va_count(__VA_ARGS__), __VA_ARGS__)
#define THREAD_ACCESS_UNLOCK(guard,...) guard.Unlock(THREAD_ACCESS_GUARD::va_count(__VA_ARGS__), __VA_ARGS__)
#define THREAD_ACCESS_RETURN(guard,ret,...) {guard.Unlock(THREAD_ACCESS_GUARD::va_count(__VA_ARGS__), __VA_ARGS__); return ret;}
#define THREAD_ACCESS_VOID_RETURN(guard,...) {guard.Unlock(THREAD_ACCESS_GUARD::va_count(__VA_ARGS__), __VA_ARGS__); return;}
#define FUNCTION_GUARD_STATE bool FUNCTION_ISGUARD = false
#define FUNCTION_GUARD_USED true
#define FUNCTION_GUARD_NOT false
#define FUNCTION_GUARD_CHECK(x) if(!FUNCTION_ISGUARD) {x;}
#define THREAD_ACCESS_GUARD_NAMEDOBJECT(x) THREAD_ACCESS_GUARD x;
#define EXTERN_THREAD_ACCESS_GUARD_NAMEDOBJECT(x) extern THREAD_ACCESS_GUARD x;
#define DEFAULT_GUARD_NAME THREAD_ACCESS_GUARD_SYSTEM
#define THREAD_ACCESS_GUARD_OBJECT THREAD_ACCESS_GUARD DEFAULT_GUARD_NAME;
#define EXTERN_THREAD_ACCESS_GUARD_OBJECT extern THREAD_ACCESS_GUARD DEFAULT_GUARD_NAME;

class THREAD_ACCESS_GUARD
{
public:
	template<typename ...Args>
	static constexpr std::size_t va_count(const Args...) { return sizeof...(Args); }

	THREAD_ACCESS_GUARD()
	{
	}
	THREAD_ACCESS_GUARD(const THREAD_ACCESS_GUARD& v)
	{
		Protector = v.Protector;
	}
	void Register(size_t count, ...)
	{
		if (count <= 0)
			return;
		va_list args;
		va_start(args, count);
		for (size_t i = 0; i < count; i++)
		{
			void* pntr = va_arg(args, void*);
			if(Protector.exist(pntr))
				throw std::runtime_error("THREAD_ACCESS ERROR: Dublicate Variable");
			Protector[pntr] = CreateMutexW(NULL, FALSE, NULL);
		}
		va_end(args);
	}
	void Release()
	{
		for (auto& i : Protector)
			CloseHandle(i.get_value());
		Protector.clear();
	}
	void Lock(size_t count, ...) const
	{
		if (count <= 0)
			return;
		va_list args;
		va_start(args, count);
		for (size_t i = 0; i < count; i++)
			WaitForSingleObject(Protector[va_arg(args, void*)], INFINITE);
		va_end(args);
	}
	void Unlock(size_t count, ...) const
	{
		if (count <= 0)
			return;
		va_list args;
		va_start(args, count);
		for (size_t i = 0; i < count; i++)
			ReleaseMutex(Protector[va_arg(args, void*)]);
		va_end(args);
	}
	void operator=(const THREAD_ACCESS_GUARD& v)
	{
		Protector = v.Protector;
	}
private:
	Map::Map<const void*, HANDLE> Protector;
};