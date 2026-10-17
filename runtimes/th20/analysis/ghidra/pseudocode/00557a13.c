/* Entry: 0x00557a13; symbol: FUN_00557a13; body bytes: 135 */

/* WARNING: Function: __EH_prolog3 replaced with injection: EH_prolog3 */
/* WARNING: Function: __EH_epilog3 replaced with injection: EH_epilog3 */
/* WARNING: Unable to track spacebase fully for stack */

__acrt_ptd * FUN_00557a13(void)

{
  __acrt_ptd *p_Var1;
  DWORD DVar2;
  int *piVar3;
  int *piVar4;
  int iStack_24;
  undefined1 auStack_20 [24];
  undefined4 local_8;
  undefined4 uStack_4;
  
  uStack_4 = 0;
  local_8 = 0x557a1f;
  piVar3 = (int *)auStack_20;
  if (DAT_005e55f8 != '\0') {
    local_8 = 0;
    if (DAT_005b2784 != -1) {
      piVar3 = &iStack_24;
      iStack_24 = DAT_005b2784;
      p_Var1 = (__acrt_ptd *)FUN_00558109();
      if (p_Var1 != (__acrt_ptd *)0x0) {
        if (p_Var1 == (__acrt_ptd *)0xffffffff) {
          p_Var1 = (__acrt_ptd *)0x0;
        }
        goto LAB_00557a92;
      }
    }
    piVar3[-1] = 0x557a51;
    p_Var1 = FUN_005578cf();
    goto LAB_00557a92;
  }
  piVar3 = &iStack_24;
  iStack_24 = 0x557a5b;
  DVar2 = GetLastError();
  local_8 = 1;
  piVar4 = piVar3;
  if (DAT_005b2784 == -1) {
LAB_00557a84:
    *(undefined4 *)((int)piVar4 + -4) = 0x557a89;
    p_Var1 = FUN_005578cf();
  }
  else {
    piVar4 = piVar3 + -1;
    piVar3[-1] = DAT_005b2784;
    piVar3[-2] = 0x557a74;
    p_Var1 = (__acrt_ptd *)FUN_00558116();
    if (p_Var1 == (__acrt_ptd *)0x0) goto LAB_00557a84;
    piVar4 = piVar3 + -1;
    if (p_Var1 == (__acrt_ptd *)0xffffffff) {
      p_Var1 = (__acrt_ptd *)0x0;
      piVar4 = piVar3 + -1;
    }
  }
  *(DWORD *)((int)piVar4 + -4) = DVar2;
  piVar3 = (int *)((int)piVar4 + -8);
  *(undefined4 *)((int)piVar4 + -8) = 0x557a92;
  SetLastError(*(DWORD *)((int)piVar4 + -4));
LAB_00557a92:
  piVar3[-1] = 0x557a99;
  return p_Var1;
}

