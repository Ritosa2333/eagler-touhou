/* Entry: 0x005414a1; symbol: ___std_fs_open_handle@16; body bytes: 52 */

/* Library Function - Single Match
    ___std_fs_open_handle@16
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

DWORD ___std_fs_open_handle_16(undefined4 *param_1,LPCWSTR param_2,DWORD param_3,DWORD param_4)

{
  HANDLE pvVar1;
  DWORD DVar2;
  
  DVar2 = 0;
  pvVar1 = CreateFileW(param_2,param_3,7,(LPSECURITY_ATTRIBUTES)0x0,3,param_4,(HANDLE)0x0);
  *param_1 = pvVar1;
  if (pvVar1 == (HANDLE)0xffffffff) {
    DVar2 = GetLastError();
  }
  return DVar2;
}

