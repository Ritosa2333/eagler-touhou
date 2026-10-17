/* Entry: 0x005479cd; symbol: ___vcrt_FlsFree; body bytes: 59 */

/* Library Function - Single Match
    ___vcrt_FlsFree
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl ___vcrt_FlsFree(DWORD param_1)

{
  FARPROC pFVar1;
  
  pFVar1 = FUN_005478a7(1,"FlsFree",(int *)&DAT_00597c00,(int *)"FlsFree");
  if (pFVar1 == (FARPROC)0x0) {
    TlsFree(param_1);
  }
  else {
    (*(code *)PTR_guard_check_icall_0056c37c)();
    (*pFVar1)();
  }
  return;
}

