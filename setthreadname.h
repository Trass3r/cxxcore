#pragma once

#if _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
static void setThreadName(uint32_t threadID, const char* threadName)
{
	#pragma pack(push, 8)
	struct THREADNAME_INFO
	{
		DWORD dwType;
		LPCSTR szName;
		DWORD dwThreadID;
		DWORD dwFlags;
	} info = { 0x1000, threadName, threadID, 0 };
	#pragma pack(pop)

	__try
	{
		RaiseException(0x406D1388, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}
static inline void setThreadName(const char* threadName)
{
	setThreadName(GetCurrentThreadId(), threadName);
}

static inline void setThreadName(HANDLE thread, const char* threadName)
{
	setThreadName(::GetThreadId(thread), threadName);
}

#else
#include <pthread.h>
static void setThreadName(pthread_t thread, const char* threadName)
{
	pthread_setname_np(thread, threadName);
}
static inline void setThreadName(const char* threadName)
{
	setThreadName(pthread_self(), threadName);
}
#endif