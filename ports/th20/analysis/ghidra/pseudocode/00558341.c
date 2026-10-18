/* Entry: 0x00558341; symbol: FUN_00558341; body bytes: 78 */

void __thiscall FUN_00558341(void *this,undefined4 param_1,undefined4 param_2)

{
  FARPROC pFVar1;
  
  if (DAT_005e607c != (FARPROC)0xffffffff) {
    pFVar1 = DAT_005e607c;
    if (DAT_005e607c == (FARPROC)0x0) {
      pFVar1 = FUN_00557e53(this,0x1f,"SystemFunction036",(int *)&DAT_00599eb8,
                            (int *)"SystemFunction036");
      if (pFVar1 == (FARPROC)0x0) goto LAB_0055838a;
    }
    (*(code *)PTR_guard_check_icall_0056c37c)(param_1,param_2);
    (*pFVar1)();
    return;
  }
LAB_0055838a:
                    /* WARNING: Subroutine does not return */
  _abort();
}

