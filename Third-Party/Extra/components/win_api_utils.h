#pragma once
#include <iostream>
#include <cstdarg>
#include <map>
#if defined(__linux__)
#include <pthread.h>
#include <chrono>
#include <thread>
#include <sys/param.h>
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
				Protector[pntr] = CreateMutexW(nullptr, FALSE, nullptr);
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

#if defined(PTHREAD_STACK_MIN)
# define SIMPLE_THREAD_STACKSIZE     MAX((((sizeof(void *) * 8 * 8) - 16) * 1024), PTHREAD_STACK_MIN)
# define SIMPLE_THREAD_STACKSIZE_LOW MAX((((sizeof(void *) * 8 * 2) - 16) * 1024), PTHREAD_STACK_MIN)
#else
# define SIMPLE_THREAD_STACKSIZE     (((sizeof(void *) * 8 * 8) - 16) * 1024)
# define SIMPLE_THREAD_STACKSIZE_LOW (((sizeof(void *) * 8 * 2) - 16) * 1024)
#endif

static inline pthread_t CreateThreadSimple(void* (*start_routine) (void*), const void* lpParameter = nullptr) {
	bool dont_use_attr = false;
	pthread_attr_t attr;
	if (pthread_attr_init(&attr)) {
		dont_use_attr = true;
	} else {
		pthread_attr_setinheritsched(&attr, PTHREAD_INHERIT_SCHED);
		pthread_attr_setstacksize(&attr, SIMPLE_THREAD_STACKSIZE);
	}

	pthread_t ret = 0;
	if (!pthread_create(&ret, dont_use_attr ? nullptr : &attr, start_routine, (void*)lpParameter)) {
		if (!dont_use_attr) {
			pthread_attr_destroy(&attr);
		}
		return ret;
	}
	else if (!dont_use_attr) {
		pthread_attr_destroy(&attr);
	}
	return 0;
}

static inline bool CreateThreadDetachedSimple(void* (*start_routine) (void*), const void* lpParameter = nullptr) {
	bool use_detach_method = false;
	pthread_attr_t attr;
	if (pthread_attr_init(&attr)) {
		use_detach_method = true;
	}

	if (!use_detach_method && pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)) {
		use_detach_method = true;
		pthread_attr_destroy(&attr);
	}
	if (!use_detach_method) {
		pthread_attr_setinheritsched(&attr, PTHREAD_INHERIT_SCHED);
		pthread_attr_setstacksize(&attr, SIMPLE_THREAD_STACKSIZE);
	}

	pthread_t ret = 0;
	if (!pthread_create(&ret, use_detach_method ? nullptr : &attr, start_routine, (void*)lpParameter)) {
		if (use_detach_method) {
			pthread_detach(ret);
		} else {
			pthread_attr_destroy(&attr);
		}
		return true;
	} else if (!use_detach_method) {
		pthread_attr_destroy(&attr);
	}
	return false;
}

static inline void wait_for(unsigned long ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

static inline void wait_for_and_close(TH_HANDLE& thread) {
	if (thread) {
		pthread_join(thread, nullptr);
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
	if (thread) {
		WaitForSingleObject(thread, INFINITE);
		CloseHandle(thread);
		thread = nullptr;
	}
}

static inline HANDLE CreateThreadSimple(
	LPTHREAD_START_ROUTINE  lpStartAddress,
	const void* lpParameter = nullptr, DWORD dwCreationFlags = 0
) {
	DWORD thid = 0;
	return ::CreateThread(nullptr, 0, lpStartAddress, (LPVOID)lpParameter, dwCreationFlags, &thid);
}

static inline bool CreateThreadDetachedSimple(
	LPTHREAD_START_ROUTINE  lpStartAddress,
	const void* lpParameter = nullptr, DWORD dwCreationFlags = 0
) {
	DWORD thid = 0;
	HANDLE th = ::CreateThread(nullptr, 0, lpStartAddress, (LPVOID)lpParameter, dwCreationFlags, &thid);
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
	LPSECURITY_ATTRIBUTES lpProcessAttributes = nullptr,
	LPSECURITY_ATTRIBUTES lpThreadAttributes = nullptr,
	bool                  bInheritHandles = false,
	LPVOID                lpEnvironment = nullptr
) {
	std::wstring cmdline;
	if (lpApplicationName.empty() || lpCommandLine.empty()) {
		cmdline = lpCommandLine;
	}
	else {
		cmdline = L"\"" + lpApplicationName + L"\" " + lpCommandLine;
	}
	return ::CreateProcessW(lpApplicationName.empty() ? nullptr : lpApplicationName.c_str(), cmdline.empty() ? nullptr : (LPWSTR)cmdline.c_str(),
		lpProcessAttributes, lpThreadAttributes, bInheritHandles ? TRUE : FALSE, dwCreationFlags, lpEnvironment,
		lpCurrentDirectory.empty() ? nullptr : lpCurrentDirectory.c_str(), lpStartupInfo, lpProcessInformation) == TRUE;
}

static inline bool CreateProcessWithLogonSimple(
	const std::wstring& lpApplicationName,
	const std::wstring& lpCommandLine,
	LPSTARTUPINFOW        lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation,
	const std::wstring& lpUsername = L"",
	const std::wstring& lpDomain = L"",
	const std::wstring& lpPassword = L"",
    DWORD dwLogonFlags = LOGON_WITH_PROFILE,
	const std::wstring& lpCurrentDirectory = L"",
	DWORD                 dwCreationFlags = 0,
	LPVOID                lpEnvironment = nullptr
) {
	std::wstring cmdline;
	if (lpApplicationName.empty() || lpCommandLine.empty()) {
		cmdline = lpCommandLine;
	}
	else {
		cmdline = L"\"" + lpApplicationName + L"\" " + lpCommandLine;
	}
	return ::CreateProcessWithLogonW(lpUsername.c_str(), lpDomain.empty() ? nullptr : lpDomain.c_str(), lpPassword.c_str(), dwLogonFlags, lpApplicationName.empty() ? nullptr : lpApplicationName.c_str(), cmdline.empty() ? nullptr : (LPWSTR)cmdline.c_str(),
		dwCreationFlags, lpEnvironment,
		lpCurrentDirectory.empty() ? nullptr : lpCurrentDirectory.c_str(), lpStartupInfo, lpProcessInformation) == TRUE;
}

static inline bool IsAppRunningAsAdminMode() {
	BOOL fIsRunAsAdmin = FALSE;
	DWORD dwError = ERROR_SUCCESS;
	PSID pAdministratorsGroup = nullptr;

	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	if (!AllocateAndInitializeSid(
		&NtAuthority,
		2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&pAdministratorsGroup)) {
		dwError = GetLastError();
		goto Cleanup;
	}

	if (!CheckTokenMembership(nullptr, pAdministratorsGroup, &fIsRunAsAdmin)) {
		dwError = GetLastError();
		goto Cleanup;
	}

Cleanup:
	if (pAdministratorsGroup) {
		FreeSid(pAdministratorsGroup);
		pAdministratorsGroup = nullptr;
	}

	if (ERROR_SUCCESS != dwError) {
		return false;
	}

	return fIsRunAsAdmin;
}
#endif
