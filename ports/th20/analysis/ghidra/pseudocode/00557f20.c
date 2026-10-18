/* Entry: 0x00557f20; symbol: try_load_library_from_system_directory; body bytes: 99 */

/* Library Function - Single Match
    struct HINSTANCE__ * __cdecl try_load_library_from_system_directory(wchar_t const * const)
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

HINSTANCE__ * __cdecl try_load_library_from_system_directory(wchar_t *param_1)

{
  HINSTANCE__ *pHVar1;
  DWORD DVar2;
  int iVar3;
  HMODULE pHVar4;
  
  pHVar1 = LoadLibraryExW(param_1,(HANDLE)0x0,0x800);
  if (pHVar1 == (HMODULE)0x0) {
    DVar2 = GetLastError();
    if (DVar2 == 0x57) {
      iVar3 = FUN_00557290((ushort *)param_1,(ushort *)L"api-ms-",7);
      if (iVar3 != 0) {
        iVar3 = FUN_00557290((ushort *)param_1,(ushort *)L"ext-ms-",7);
        if (iVar3 != 0) {
          pHVar4 = LoadLibraryExW(param_1,(HANDLE)0x0,0);
          return pHVar4;
        }
      }
    }
    pHVar1 = (HINSTANCE__ *)0x0;
  }
  return pHVar1;
}

