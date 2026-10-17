/* Entry: 0x00554d40; symbol: FUN_00554d40; body bytes: 101 */

uint __cdecl FUN_00554d40(uint param_1,uint param_2)

{
  __acrt_ptd *p_Var1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  
  if ((param_2 == 0) || ((param_2 & param_2 - 1) != 0)) {
    p_Var1 = FUN_005516c1();
    *(undefined4 *)p_Var1 = 0x16;
    FUN_005480bc();
  }
  else {
    uVar3 = 4;
    if (4 < param_2) {
      uVar3 = param_2;
    }
    uVar4 = param_1 + 3 + uVar3;
    if (uVar4 < param_1) {
      p_Var1 = FUN_005516c1();
      *(undefined4 *)p_Var1 = 0xc;
    }
    else {
      iVar2 = FUN_0054a550(uVar4);
      if (iVar2 != 0) {
        uVar3 = uVar3 + 3 + iVar2 & -uVar3;
        *(int *)(uVar3 - 4) = iVar2;
        return uVar3;
      }
    }
  }
  return 0;
}

