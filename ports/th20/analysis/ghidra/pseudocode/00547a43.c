/* Entry: 0x00547a43; symbol: ___vcrt_FlsSetValue; body bytes: 62 */

/* Library Function - Single Match
    ___vcrt_FlsSetValue
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl ___vcrt_FlsSetValue(DWORD param_1,LPVOID param_2)

{
  FARPROC pFVar1;
  
  pFVar1 = FUN_005478a7(3,"FlsSetValue",(int *)&DAT_00597c24,(int *)"FlsSetValue");
  if (pFVar1 == (FARPROC)0x0) {
    TlsSetValue(param_1,param_2);
  }
  else {
    (*(code *)PTR_guard_check_icall_0056c37c)();
    (*pFVar1)();
  }
  return;
}

