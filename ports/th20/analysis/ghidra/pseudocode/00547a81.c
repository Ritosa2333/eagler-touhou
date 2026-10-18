/* Entry: 0x00547a81; symbol: ___vcrt_InitializeCriticalSectionEx; body bytes: 71 */

/* Library Function - Single Match
    ___vcrt_InitializeCriticalSectionEx
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl
___vcrt_InitializeCriticalSectionEx(LPCRITICAL_SECTION param_1,DWORD param_2,undefined4 param_3)

{
  FARPROC pFVar1;
  
  pFVar1 = FUN_005478a7(4,"InitializeCriticalSectionEx",(int *)&DAT_00597c38,
                        (int *)"InitializeCriticalSectionEx");
  if (pFVar1 == (FARPROC)0x0) {
    InitializeCriticalSectionAndSpinCount(param_1,param_2);
  }
  else {
    (*(code *)PTR_guard_check_icall_0056c37c)(param_1,param_2,param_3);
    (*pFVar1)();
  }
  return;
}

