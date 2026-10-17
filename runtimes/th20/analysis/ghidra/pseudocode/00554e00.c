/* Entry: 0x00554e00; symbol: FUN_00554e00; body bytes: 135 */

int __cdecl FUN_00554e00(uint param_1,uint param_2,uint param_3)

{
  __acrt_ptd *p_Var1;
  int iVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  
  if (((param_2 == 0) || ((param_2 & param_2 - 1) != 0)) || ((param_3 != 0 && (param_1 <= param_3)))
     ) {
    p_Var1 = FUN_005516c1();
    *(undefined4 *)p_Var1 = 0x16;
    FUN_005480bc();
  }
  else {
    uVar6 = 4;
    if (4 < param_2) {
      uVar6 = param_2;
    }
    uVar5 = -param_3 & 3;
    uVar4 = uVar5 + 3 + uVar6 + param_1;
    if (uVar4 < param_1) {
      p_Var1 = FUN_005516c1();
      *(undefined4 *)p_Var1 = 0xc;
    }
    else {
      iVar2 = FUN_0054a550(uVar4);
      if (iVar2 != 0) {
        iVar3 = (uVar5 + 3 + uVar6 + iVar2 + param_3 & -uVar6) - param_3;
        *(int *)((iVar3 - uVar5) + -4) = iVar2;
        return iVar3;
      }
    }
  }
  return 0;
}

