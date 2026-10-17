/* Entry: 0x00566968; symbol: ___dcrt_write_console; body bytes: 85 */

/* Library Function - Single Match
    ___dcrt_write_console
   
   Libraries: Visual Studio 2019 Debug, Visual Studio 2019 Release */

BOOL __cdecl ___dcrt_write_console(void *param_1,DWORD param_2,LPDWORD param_3)

{
  BOOL BVar1;
  DWORD DVar2;
  
  BVar1 = WriteConsoleW(DAT_005b3020,param_1,param_2,param_3,(LPVOID)0x0);
  if (BVar1 == 0) {
    DVar2 = GetLastError();
    if (DVar2 == 6) {
      FID_conflict____dcrt_terminate_console_output();
      __dcrt_lowio_initialize_console_output();
      BVar1 = WriteConsoleW(DAT_005b3020,param_1,param_2,param_3,(LPVOID)0x0);
    }
  }
  return BVar1;
}

