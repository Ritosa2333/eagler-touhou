#pragma once
#include "windows.h"
using BCRYPT_ALG_HANDLE=void*;
inline constexpr wchar_t BCRYPT_SHA256_ALGORITHM[]=L"SHA256";
inline int BCryptOpenAlgorithmProvider(BCRYPT_ALG_HANDLE *out,const wchar_t*,const wchar_t*,DWORD){*out=out;return 0;}
inline int BCryptCloseAlgorithmProvider(BCRYPT_ALG_HANDLE,DWORD){return 0;}
int BCryptHash(BCRYPT_ALG_HANDLE,PUCHAR,ULONG,PUCHAR,ULONG,PUCHAR,ULONG);
