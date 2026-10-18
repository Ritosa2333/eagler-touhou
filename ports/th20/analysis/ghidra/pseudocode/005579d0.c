/* Entry: 0x005579d0; symbol: FUN_005579d0; body bytes: 61 */

/* WARNING: Function: __EH_prolog3 replaced with injection: EH_prolog3 */
/* WARNING: Function: __EH_epilog3 replaced with injection: EH_epilog3 */
/* WARNING: Unable to track spacebase fully for stack */

void FUN_005579d0(void)

{
  int iVar1;
  __acrt_ptd *p_Var2;
  int *piVar3;
  int iStack_24;
  undefined1 auStack_20 [24];
  undefined4 local_8;
  undefined4 uStack_4;
  
  uStack_4 = 0;
  piVar3 = (int *)auStack_20;
  local_8 = 0;
  if (DAT_005b2784 != -1) {
    piVar3 = &iStack_24;
    iStack_24 = DAT_005b2784;
    iVar1 = FUN_00558116();
    if (iVar1 != 0) {
      if (iVar1 == -1) goto LAB_00557a08;
      goto LAB_005579f9;
    }
  }
  piVar3[-1] = 0x557a04;
  p_Var2 = FUN_005578cf();
  if (p_Var2 == (__acrt_ptd *)0x0) {
LAB_00557a08:
                    /* WARNING: Subroutine does not return */
    piVar3[-1] = (int)&UNK_00557a0d;
    _abort();
  }
LAB_005579f9:
  piVar3[-1] = 0x5579fe;
  return;
}

