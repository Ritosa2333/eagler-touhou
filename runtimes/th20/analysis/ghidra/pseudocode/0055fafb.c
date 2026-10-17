/* Entry: 0x0055fafb; symbol: FUN_0055fafb; body bytes: 145 */

undefined4 __cdecl FUN_0055fafb(uint param_1)

{
  int iVar1;
  __acrt_ptd *p_Var2;
  int iVar3;
  DWORD nStdHandle;
  
  if ((-1 < (int)param_1) && (param_1 < DAT_005e5890)) {
    iVar3 = (param_1 & 0x3f) * 0x38;
    if (((*(byte *)(iVar3 + 0x28 + (&DAT_005e5690)[param_1 >> 6]) & 1) != 0) &&
       (*(int *)(iVar3 + 0x18 + (&DAT_005e5690)[param_1 >> 6]) != -1)) {
      iVar1 = FUN_005558f0();
      if (iVar1 == 1) {
        if (param_1 == 0) {
          nStdHandle = 0xfffffff6;
        }
        else if (param_1 == 1) {
          nStdHandle = 0xfffffff5;
        }
        else {
          if (param_1 != 2) goto LAB_0055fb61;
          nStdHandle = 0xfffffff4;
        }
        SetStdHandle(nStdHandle,(HANDLE)0x0);
      }
LAB_0055fb61:
      *(undefined4 *)((&DAT_005e5690)[param_1 >> 6] + 0x18 + iVar3) = 0xffffffff;
      return 0;
    }
  }
  p_Var2 = FUN_005516c1();
  *(undefined4 *)p_Var2 = 9;
  p_Var2 = FUN_005516ac();
  *(undefined4 *)p_Var2 = 0;
  return 0xffffffff;
}

