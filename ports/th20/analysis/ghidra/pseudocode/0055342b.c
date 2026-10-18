/* Entry: 0x0055342b; symbol: free_environment<>; body bytes: 47 */

/* Library Function - Multiple Matches With Same Base Name
    void __cdecl free_environment<char>(char * * const)
    void __cdecl free_environment<wchar_t>(wchar_t * * const)
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl free_environment<>(undefined4 *param_1)

{
  LPVOID pvVar1;
  undefined4 *puVar2;
  
  if (param_1 != (undefined4 *)0x0) {
    pvVar1 = (LPVOID)*param_1;
    puVar2 = param_1;
    while (pvVar1 != (LPVOID)0x0) {
      FUN_00557ba0(pvVar1);
      puVar2 = puVar2 + 1;
      pvVar1 = (LPVOID)*puVar2;
    }
    FUN_00557ba0(param_1);
  }
  return;
}

