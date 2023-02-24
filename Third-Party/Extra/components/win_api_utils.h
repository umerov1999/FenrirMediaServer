#pragma once
#include <iostream>
#include <Windows.h>

static inline HANDLE CreateThreadSimple(
	LPTHREAD_START_ROUTINE  lpStartAddress,
	const void* lpParameter = NULL, DWORD dwCreationFlags = 0
) {
	DWORD thid = 0;
	return ::CreateThread(NULL, 0, lpStartAddress, (LPVOID)lpParameter, dwCreationFlags, &thid);
}

template<typename T>
static inline T extractThreadParamAndDelete(const void* lpParameter) {
	T* pointer = ((T*)lpParameter);
	T ret = *pointer;
	delete pointer;
	return ret;
}

static inline bool CreateProcessSimple(
	const std::wstring &lpApplicationName,
	const std::wstring &lpCommandLine,
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