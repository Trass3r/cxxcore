#include <typeinfo>
#include <stdio.h>
void PreviewException(void * pObject,
	void * pThrowAddress,
	const type_info* pObjectInfo)
{
	(void)pObject;
	if (pObjectInfo)
		printf("Exception of type \"%s\" occured @ %p\n",
		pObjectInfo->name(),
		pThrowAddress);
}

#include <stdexcept>

#define NOMINMAX
#include <windows.h>

const DWORD NT_EXCEPTION = 0xE06D7363; // 'msc' | 0xE0000000
const DWORD EH_MAGIC1 = 0x19930520;
const DWORD EH_PURE_MAGIC1 = 0x01994000;

template <typename T>
struct pointer
{
#if 0
	T* p = nullptr;
	operator T*() { return p; }
#else
	void*   base = nullptr;
	__int32 off = 0;

	operator T*() { return (T*)(base + off); }
#endif
};

/* standard implementation
extern "C"
void __stdcall _CxxThrowException(void * pObject,
_s__ThrowInfo const * pObjectInfo)
{
	const ULONG_PTR args[] = { MS_MAGIC,
		(ULONG_PTR)pObject,
		(ULONG_PTR)pObjectInfo };
	RaiseException(CPP_EXCEPTION,
		EXCEPTION_NONCONTINUABLE,
		sizeof(args) / sizeof(args[0]),
		args);

		{
			EH_MAGIC_NUMBER1,
			pExceptionObject,
		pTI,
		#if _EH_RELATIVE_OFFSETS
			ThrowImageBase                        // Image base of thrown object
		#endif
		}
}*/


#pragma intrinsic(_ReturnAddress)

// inhibit optimization of frames
//#pragma optimize("y",off)
typedef unsigned __int64 *      PULONG_PTR;
#if (defined(_M_IA64) || defined(_M_AMD64) || defined(_M_ARM_NT) || defined(_M_ARM64))
#define _EH_RELATIVE_OFFSETS 1
#else
#define _EH_RELATIVE_OFFSETS 0
#endif

#if _EH_RELATIVE_OFFSETS && !defined(VERSP_IA64) && !defined(_M_CEE_PURE)/*IFSTRIP=IGN*/
typedef	__int32	PMFN;					// Image relative offset of Member Function
#else
typedef void(*_EH_PTR64 PMFN)(void* _EH_PTR64);
#endif
typedef const struct _s_CatchableTypeArray {
	int	nCatchableTypes;
#if _EH_RELATIVE_OFFSETS && !defined(VERSP_IA64) && !defined(_M_CEE_PURE)	/*IFSTRIP=IGN*/
	__int32			arrayOfCatchableTypes[];	// Image relative offset of Catchable Types
#else
	CatchableType	* _EH_PTR64 arrayOfCatchableTypes[];
#endif
} CatchableTypeArray;

typedef const struct _s_ThrowInfo {
	unsigned int	attributes;			// Throw Info attributes (Bit field)
	PMFN			pmfnUnwind;			// Destructor to call when exception
	// has been handled or aborted.
#if _EH_RELATIVE_OFFSETS && !defined(VERSP_IA64) && !defined(_M_CEE_PURE)	/*IFSTRIP=IGN*/
	__int32			pForwardCompat;		// Image relative offset of Forward compatibility frame handler
	__int32			pCatchableTypeArray;// Image relative offset of CatchableTypeArray
#else
#if defined(__cplusplus)
	int(__cdecl* pForwardCompat)(...);	// Forward compatibility frame handler
#else
	int(__cdecl* pForwardCompat)();	// Forward compatibility frame handler
#endif
	CatchableTypeArray* pCatchableTypeArray;	// Pointer to list of pointers to types.
#endif
} ThrowInfo;

__declspec (noreturn)
extern "C"
//__declspec(dllimport)
extern void __stdcall __imp__CxxThrowException(void* pExceptionObject,
_ThrowInfo* pThrowInfo);

__declspec (noreturn)
extern "C"
void __stdcall _CxxThrowException(void* pExceptionObject,
                                  _ThrowInfo* pThrowInfo)
{
	__imp__CxxThrowException(pExceptionObject, pThrowInfo);
	ThrowInfo* pTI = (ThrowInfo*)pThrowInfo;

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

	void * paddr = _ReturnAddress();
	// call __CxxThrowException@8 is 5 bytes
	// long on i386. Subtracting 5 from return address
	// gives us the address of call instruction
	paddr = (void*)((size_t)paddr - 5);

	/*
	type_info * pti = 0;
	// since we can get here when an exception is rethrown
	// pObjectInfo could be 0. Also it is a good idea to
	// make a sanity check
	if (pTI &&
		pTI->pCatchableTypeArray &&
		((CatchableTypeArray*)((size_t)ThrowImageBase + (pTI->pCatchableTypeArray)))->nCatchableTypes > 0
//		pTI->pCatchableTypeArray->nCatchableTypes > 0
	)
	{
		const CatchableTypeArray * pTypes =
			((CatchableTypeArray*)((size_t)ThrowImageBase + pTI->pCatchableTypeArray));
		pti = (type_info*)
			((CatchableType*)((size_t)ThrowImageBase + (pTypes->arrayOfCatchableTypes)))[0].pType
			//((*(pTypes->arrayOfCatchableTypes))[0].pType)
			;
	}

	//call the hook function
	PreviewException(pExceptionObject, paddr, pti);


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




	/*




	void * paddr = _ReturnAddress();
	// call __CxxThrowException@8 is 5 bytes
	// long on i386. Subtracting 5 from return address
	// gives us the address of call instruction
	paddr = (void*)((size_t)paddr - 5);
//	void * pframe = GetCallerFrame();

	type_info * pti = 0;
	// since we can get here when an exception is rethrown
	// pObjectInfo could be 0. Also it is a good idea to
	// make a sanity check
	if (pObjectInfo &&
		pObjectInfo->pCatchableTypeArray &&
		pObjectInfo->pCatchableTypeArray->nCatchableTypes > 0)
	{
		const _s__CatchableTypeArray * pTypes =
			pObjectInfo->pCatchableTypeArray;
		pti = (type_info*)
			((*(pTypes->arrayOfCatchableTypes))[0].pType);
	}

	//call the hook function
	PreviewException(pObject, paddr, pti);

	const ULONG_PTR args[] = { MS_MAGIC,
		(ULONG_PTR)pObject,
		(ULONG_PTR)pObjectInfo };
	RaiseException(CPP_EXCEPTION,
		EXCEPTION_NONCONTINUABLE,
		sizeof(args) / sizeof(args[0]),
		args);
	*/
}

//restore optimization settings
#pragma optimize("",on)
void foo()
{
	throw std::runtime_error("bla");
}

int main()
{
	try
	{
		foo();
	}
	catch (std::exception& e)
	{
		printf("%s\n", e.what());
	}

	return 0;
}