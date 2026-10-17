/* Entry: 0x00547a08; symbol: ___vcrt_FlsGetValue; body bytes: 59 */

/* Library Function - Single Match
    ___vcrt_FlsGetValue
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl ___vcrt_FlsGetValue(DWORD param_1)

{
  FARPROC pFVar1;
  
  pFVar1 = FUN_005478a7(2,"FlsGetValue",(int *)&DAT_00597c10,(int *)"FlsGetValue");
  if (pFVar1 == (FARPROC)0x0) {
    TlsGetValue(param_1);
  }
  else {
    (*(code *)PTR_guard_check_icall_0056c37c)();
    (*pFVar1)();
  }
  return;
}

