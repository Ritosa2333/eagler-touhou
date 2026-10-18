/* Entry: 0x005582ba; symbol: FUN_005582ba; body bytes: 74 */

int __thiscall FUN_005582ba(void *this,undefined4 param_1)

{
  FARPROC pFVar1;
  int iVar2;
  
  if (DAT_005e6060 == (FARPROC)0xffffffff) {
LAB_005582fd:
    iVar2 = 0;
  }
  else {
    pFVar1 = DAT_005e6060;
    if (DAT_005e6060 == (FARPROC)0x0) {
      pFVar1 = FUN_00557e53(this,0x18,"RoInitialize",(int *)&DAT_00599e38,(int *)"RoInitialize");
      if (pFVar1 == (FARPROC)0x0) goto LAB_005582fd;
    }
    (*(code *)PTR_guard_check_icall_0056c37c)(param_1);
    iVar2 = (*pFVar1)();
  }
  return iVar2;
}

