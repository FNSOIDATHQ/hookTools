#ifndef __HOOKTOOL_H__
#define __HOOKTOOL_H__

//cpp std library+windows sdk
#include <windows.h>
#include <cstdint>

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif

#define DLL_IMPORT __declspec(dllimport)

#ifdef __cplusplus
extern "C"
{
#endif
//hook tool
DLL_EXPORT void hookJMP(void*,void*);
DLL_EXPORT void* hookVTable(void**,size_t,void*);
DLL_EXPORT void writeVarToAddress(UINT,UINT,void*);
DLL_EXPORT void writeVarToAddressP(void*,UINT,void*);
DLL_EXPORT void writeNopsToAddress(UINT,UINT);
DLL_EXPORT void* getClassFunctionAddress(DWORD*,int);
DLL_EXPORT UINT getThisPtrFromECX();
DLL_EXPORT void moveVarToECX(UINT);

//TODO:
DLL_EXPORT void hookTrampoline(void*, void*,void**);

#ifdef __cplusplus
}
#endif

#endif // __HOOKTOOL_H__
