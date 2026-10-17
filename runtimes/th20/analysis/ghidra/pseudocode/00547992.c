/* Entry: 0x00547992; symbol: ___vcrt_FlsAlloc; body bytes: 59 */

/* Library Function - Single Match
    ___vcrt_FlsAlloc
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl ___vcrt_FlsAlloc(undefined4 param_1)

{
  FARPROC pFVar1;
  
  pFVar1 = FUN_005478a7(0,"FlsAlloc",(int *)&DAT_00597bec,(int *)"FlsAlloc");
  if (pFVar1 != (FARPROC)0x0) {
    (*(code *)PTR_guard_check_icall_0056c37c)(param_1);
    (*pFVar1)();
    return;
  }
                    /* WARNING: Could not recover jumptable at 0x005479c7. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  TlsAlloc();
  return;
}

