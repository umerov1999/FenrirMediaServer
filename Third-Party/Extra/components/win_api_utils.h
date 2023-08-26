#pragma once
#include <iostream>
#include <cstdarg>
#include <map>
#if defined(__linux__)
#include <pthread.h>
#include <chrono>
#include <thread>
#else
#include <Windows.h>
#endif

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

template<typename T>
static inline T extractThreadParamAndDelete(const void* lpParameter) {
	T* pointer = ((T*)lpParameter);
	T ret = *pointer;
	delete pointer;
	return ret;
}

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
			size_t pntr = reinterpret_cast<size_t>(va_arg(args, void*));
			if (Protector.find(pntr) == Protector.end()) {
#if defined(__linux__)
				Protector[pntr] = new pthread_mutex_t();
				pthread_mutex_init(Protector[pntr], nullptr);
#else
				Protector[pntr] = CreateMutexW(NULL, FALSE, NULL);
#endif
			}
		}
		va_end(args);
	}
	void Release()
	{
		for (auto& i : Protector) {
#if defined(__linux__)
			pthread_mutex_destroy(i.second);
			delete i.second;
#else
			CloseHandle(i.second);
#endif
		}
		Protector.clear();
	}
	void Lock(size_t count, ...) const
	{
		if (count <= 0)
			return;
		va_list args;
		va_start(args, count);
		for (size_t i = 0; i < count; i++) {
#if defined(__linux__)
			pthread_mutex_lock(Protector.find(reinterpret_cast<size_t>(va_arg(args, void*)))->second);
#else
			WaitForSingleObject(Protector.find(reinterpret_cast<size_t>(va_arg(args, void*)))->second, INFINITE);
#endif
		}
		va_end(args);
	}
	void Unlock(size_t count, ...) const
	{
		if (count <= 0)
			return;
		va_list args;
		va_start(args, count);
		for (size_t i = 0; i < count; i++) {
#if defined(__linux__)
			pthread_mutex_unlock(Protector.find(reinterpret_cast<size_t>(va_arg(args, void*)))->second);
#else
			ReleaseMutex(Protector.find(reinterpret_cast<size_t>(va_arg(args, void*)))->second);
#endif
		}
		va_end(args);
	}
	void operator=(const THREAD_ACCESS_GUARD& v)
	{
		Protector = v.Protector;
	}
private:
#if defined(__linux__)
	std::map<size_t, pthread_mutex_t*> Protector;
#else
	std::map<size_t, HANDLE> Protector;
#endif
};

#if defined(__linux__)
#define ZR_P 0
#define NULL_P nullptr

typedef int SOCKET_C;
typedef pthread_t TH_HANDLE;
#define closesocket close
#define INVALID_SOCKET 0
#define SOCKET_ERROR -1
#define THREAD_IMPL(name,arg) void* name(void* arg)

static inline pthread_t CreateThreadSimple(void* (*start_routine) (void*), const void* lpParameter = nullptr) {
	pthread_t ret = 0;
	int thid = pthread_create(&ret, NULL, start_routine, (void*)lpParameter);
	if (!thid) {
		return ret;
	}
	else {
		return 0;
	}
}

static inline bool CreateThreadDetachedSimple(void* (*start_routine) (void*), const void* lpParameter = nullptr) {
	pthread_t ret = 0;
	int thid = pthread_create(&ret, NULL, start_routine, (void*)lpParameter);
	if (!thid) {
		pthread_detach(thid);
		return true;
	}
	return false;
}

static inline void wait_for(unsigned long ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

static inline void wait_for_and_close(TH_HANDLE& thread) {
	if (thread != 0) {
		pthread_join(thread, NULL);
		thread = 0;
	}
}

#else
#define ZR_P nullptr
#define NULL_P 0

typedef SOCKET SOCKET_C;
typedef HANDLE TH_HANDLE;
#define SHUT_RDWR SD_BOTH
#define THREAD_IMPL(name,arg) DWORD WINAPI name(LPVOID arg)

static inline void wait_for(unsigned long ms) {
	Sleep(ms);
}

static inline void wait_for_and_close(TH_HANDLE& thread) {
	if (thread != NULL) {
		WaitForSingleObject(thread, INFINITE);
		CloseHandle(thread);
		thread = NULL;
	}
}

static inline HANDLE CreateThreadSimple(
	LPTHREAD_START_ROUTINE  lpStartAddress,
	const void* lpParameter = NULL, DWORD dwCreationFlags = 0
) {
	DWORD thid = 0;
	return ::CreateThread(NULL, 0, lpStartAddress, (LPVOID)lpParameter, dwCreationFlags, &thid);
}

static inline bool CreateThreadDetachedSimple(
	LPTHREAD_START_ROUTINE  lpStartAddress,
	const void* lpParameter = NULL, DWORD dwCreationFlags = 0
) {
	DWORD thid = 0;
	HANDLE th = ::CreateThread(NULL, 0, lpStartAddress, (LPVOID)lpParameter, dwCreationFlags, &thid);
	if(!th) {
		return false;
	}
	CloseHandle(th);
	return true;
}

static inline bool CreateProcessSimple(
	const std::wstring& lpApplicationName,
	const std::wstring& lpCommandLine,
	LPSTARTUPINFOW        lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation,
	const std::wstring& lpCurrentDirectory = L"",
	DWORD                 dwCreationFlags = 0,
	LPSECURITY_ATTRIBUTES lpProcessAttributes = NULL,
	LPSECURITY_ATTRIBUTES lpThreadAttributes = NULL,
	bool                  bInheritHandles = false,
	LPVOID                lpEnvironment = NULL
) {
	std::wstring cmdline;
	if (lpApplicationName.empty() || lpCommandLine.empty()) {
		cmdline = lpCommandLine;
	}
	else {
		cmdline = L"\"" + lpApplicationName + L"\" " + lpCommandLine;
	}
	return ::CreateProcessW(lpApplicationName.empty() ? NULL : lpApplicationName.c_str(), cmdline.empty() ? NULL : (LPWSTR)cmdline.c_str(),
		lpProcessAttributes, lpThreadAttributes, bInheritHandles ? TRUE : FALSE, dwCreationFlags, lpEnvironment,
		lpCurrentDirectory.empty() ? NULL : lpCurrentDirectory.c_str(), lpStartupInfo, lpProcessInformation) == TRUE;
}
#endif
