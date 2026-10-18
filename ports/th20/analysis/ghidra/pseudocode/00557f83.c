/* Entry: 0x00557f83; symbol: FUN_00557f83; body bytes: 79 */

int __thiscall FUN_00557f83(void *this,undefined4 param_1)

{
  FARPROC pFVar1;
  int iVar2;
  
  if (DAT_005e6068 == (FARPROC)0xffffffff) {
LAB_00557fc8:
    iVar2 = -0x3ffffddb;
  }
  else {
    pFVar1 = DAT_005e6068;
    if (DAT_005e6068 == (FARPROC)0x0) {
      pFVar1 = FUN_00557e53(this,0x1a,"AppPolicyGetProcessTerminationMethod",(int *)&DAT_00599e60,
                            (int *)"AppPolicyGetProcessTerminationMethod");
      if (pFVar1 == (FARPROC)0x0) goto LAB_00557fc8;
    }
    (*(code *)PTR_guard_check_icall_0056c37c)(0xfffffffa,param_1);
    iVar2 = (*pFVar1)();
  }
  return iVar2;
}

