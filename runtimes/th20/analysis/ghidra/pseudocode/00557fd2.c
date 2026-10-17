/* Entry: 0x00557fd2; symbol: FUN_00557fd2; body bytes: 79 */

int __thiscall FUN_00557fd2(void *this,undefined4 param_1)

{
  FARPROC pFVar1;
  int iVar2;
  
  if (DAT_005e606c == (FARPROC)0xffffffff) {
LAB_00558017:
    iVar2 = -0x3ffffddb;
  }
  else {
    pFVar1 = DAT_005e606c;
    if (DAT_005e606c == (FARPROC)0x0) {
      pFVar1 = FUN_00557e53(this,0x1b,"AppPolicyGetThreadInitializationType",(int *)&DAT_00599e8c,
                            (int *)"AppPolicyGetThreadInitializationType");
      if (pFVar1 == (FARPROC)0x0) goto LAB_00558017;
    }
    (*(code *)PTR_guard_check_icall_0056c37c)(0xfffffffa,param_1);
    iVar2 = (*pFVar1)();
  }
  return iVar2;
}

