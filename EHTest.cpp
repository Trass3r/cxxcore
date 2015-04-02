// http://stacktrace.sourceforge.net

#ifdef _WIN32

#define NOMINMAX
#include <windows.h>
#include "call_stack.hpp"


const DWORD NT_EXCEPTION = 0xE06D7363; // 'msc' | 0xE0000000
const DWORD EH_MAGIC1 = 0x19930520;
const DWORD EH_PURE_MAGIC1 = 0x01994000;

// inhibit optimization of frames
//#pragma optimize("y",off)
typedef unsigned __int64 * PULONG_PTR;
#if (defined(_M_IA64) || defined(_M_AMD64) || defined(_M_ARM_NT) || defined(_M_ARM64))
#define _EH_RELATIVE_OFFSETS 1
#else
#define _EH_RELATIVE_OFFSETS 0
#endif

__declspec (noreturn)
extern "C"
void __stdcall _CxxThrowException(void* pExceptionObject,
                                  _ThrowInfo* pTI)
{
#if _EH_RELATIVE_OFFSETS
	PVOID ThrowImageBase = RtlPcToFileHeader((PVOID)pTI, &ThrowImageBase);
#else
	PVOID ThrowImageBase = nullptr;
#endif

	bool pureModule = pTI && (
		((*pTI).attributes & 8) //TI_IsPure)
#if _EH_RELATIVE_OFFSETS
		|| !ThrowImageBase
#endif
		);

	stacktrace::call_stack cs(1);
	std::string s = cs.to_string();

	printf("\n--------\nException occured: %s\n--------\n", s.c_str());

	const ULONG_PTR args[] = { pureModule ? EH_PURE_MAGIC1 : EH_MAGIC1,
	                           (ULONG_PTR)pExceptionObject,
	                           (ULONG_PTR)pTI
#if _EH_RELATIVE_OFFSETS
	                           ,(ULONG_PTR)ThrowImageBase                        // Image base of thrown object
#endif
	};

	RaiseException(NT_EXCEPTION,
		EXCEPTION_NONCONTINUABLE,
		sizeof(args) / sizeof(args[0]),
		args);
}

//restore optimization settings
#pragma optimize("",on)

#endif
