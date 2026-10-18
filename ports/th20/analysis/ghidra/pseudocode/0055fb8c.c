/* Entry: 0x0055fb8c; symbol: FUN_0055fb8c; body bytes: 106 */

undefined4 __cdecl FUN_0055fb8c(uint param_1)

{
  __acrt_ptd *p_Var1;
  int iVar2;
  
  if (param_1 == 0xfffffffe) {
    p_Var1 = FUN_005516ac();
    *(undefined4 *)p_Var1 = 0;
    p_Var1 = FUN_005516c1();
    *(undefined4 *)p_Var1 = 9;
  }
  else {
    if ((-1 < (int)param_1) && (param_1 < DAT_005e5890)) {
      iVar2 = (param_1 & 0x3f) * 0x38;
      if ((*(byte *)((&DAT_005e5690)[param_1 >> 6] + 0x28 + iVar2) & 1) != 0) {
        return *(undefined4 *)((&DAT_005e5690)[param_1 >> 6] + 0x18 + iVar2);
      }
    }
    p_Var1 = FUN_005516ac();
    *(undefined4 *)p_Var1 = 0;
    p_Var1 = FUN_005516c1();
    *(undefined4 *)p_Var1 = 9;
    FUN_005480bc();
  }
  return 0xffffffff;
}

