/* Entry: 0x00557a9f; symbol: FUN_00557a9f; body bytes: 68 */

/* WARNING: Function: __EH_prolog3 replaced with injection: EH_prolog3 */
/* WARNING: Function: __EH_epilog3 replaced with injection: EH_epilog3 */
/* WARNING: Unable to track spacebase fully for stack */

__acrt_ptd * __cdecl FUN_00557a9f(undefined4 param_1,int param_2)

{
  __acrt_ptd *p_Var1;
  int *piVar2;
  __acrt_ptd *p_Var3;
  bool bVar4;
  int iStack_24;
  undefined1 auStack_20 [24];
  undefined4 local_8;
  undefined4 uStack_4;
  
  uStack_4 = 0;
  piVar2 = (int *)auStack_20;
  p_Var3 = (__acrt_ptd *)0x0;
  local_8 = 0;
  if (DAT_005b2784 != -1) {
    piVar2 = &iStack_24;
    iStack_24 = DAT_005b2784;
    p_Var1 = (__acrt_ptd *)FUN_00558116();
    if (p_Var1 != (__acrt_ptd *)0x0) {
      bVar4 = p_Var1 == (__acrt_ptd *)0xffffffff;
      goto LAB_00557ad0;
    }
  }
  piVar2[-1] = 0x557ace;
  p_Var1 = FUN_005578cf();
  bVar4 = p_Var1 == (__acrt_ptd *)0x0;
LAB_00557ad0:
  if (!bVar4) {
    p_Var3 = p_Var1 + param_2 * 0x364;
  }
  piVar2[-1] = 0x557ae2;
  return p_Var3;
}

