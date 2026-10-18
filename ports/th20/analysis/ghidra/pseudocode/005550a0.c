/* Entry: 0x005550a0; symbol: FUN_005550a0; body bytes: 658 */

uint * __cdecl FUN_005550a0(uint *param_1,uint param_2,uint param_3,uint param_4,uint param_5)

{
  LPCVOID pvVar1;
  undefined4 uVar2;
  bool bVar3;
  __acrt_ptd *p_Var4;
  SIZE_T SVar5;
  uint uVar6;
  uint uVar7;
  int iVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  LPCVOID local_18;
  LPVOID local_8;
  
  if ((param_2 != 0) && (0xffffffe0 / param_2 < param_3)) {
    p_Var4 = FUN_005516c1();
    *(undefined4 *)p_Var4 = 0xc;
    return (uint *)0x0;
  }
  uVar11 = param_2 * param_3;
  if (param_1 == (uint *)0x0) {
    uVar12 = 0;
    if (((param_4 == 0) || ((param_4 & param_4 - 1) != 0)) ||
       ((param_5 != 0 && (uVar11 <= param_5)))) {
      p_Var4 = FUN_005516c1();
      *(undefined4 *)p_Var4 = 0x16;
      FUN_005480bc();
LAB_00555310:
      param_1 = (uint *)0x0;
    }
    else {
      uVar6 = 4;
      if (4 < param_4) {
        uVar6 = param_4;
      }
      iVar8 = (-param_5 & 3) + 3 + uVar6;
      uVar9 = iVar8 + uVar11;
      if (uVar9 <= uVar11 && uVar11 - uVar9 != 0) {
        p_Var4 = FUN_005516c1();
        *(undefined4 *)p_Var4 = 0xc;
        goto LAB_00555310;
      }
      local_18 = (LPCVOID)FUN_0054a550(uVar9);
      if (local_18 == (LPCVOID)0x0) goto LAB_00555310;
      param_1 = (uint *)(((int)local_18 + param_5 + iVar8 & -uVar6) - param_5);
      iVar8 = (int)param_1 - (-param_5 & 3);
LAB_005552fb:
      *(LPCVOID *)(iVar8 + -4) = local_18;
    }
    if (param_1 == (uint *)0x0) {
      return (uint *)0x0;
    }
LAB_00555316:
    if (uVar12 < uVar11) {
      _memset((void *)((int)param_1 + uVar12),0,uVar11 - uVar12);
    }
  }
  else {
    SVar5 = __msize_base(*(LPCVOID *)(((uint)param_1 & 0xfffffffc) - 4));
    uVar12 = 4;
    if (4 < param_4) {
      uVar12 = param_4;
    }
    uVar12 = SVar5 + ((-3 - uVar12) - (-param_5 & 3));
    if (uVar11 == 0) {
      thunk_FUN_00557ba0(*(LPVOID *)(((uint)param_1 & 0xfffffffc) - 4));
    }
    else {
      if (((param_4 != 0) && ((param_4 & param_4 - 1) == 0)) &&
         ((param_5 == 0 || (param_5 < uVar11)))) {
        pvVar1 = *(LPCVOID *)(((uint)param_1 & 0xfffffffc) - 4);
        uVar6 = 4;
        if (4 < param_4) {
          uVar6 = param_4;
        }
        uVar7 = -param_5 & 3;
        iVar8 = (int)param_1 - (int)pvVar1;
        SVar5 = __msize_base(pvVar1);
        uVar9 = (int)pvVar1 + (SVar5 - (int)param_1);
        if (uVar11 < uVar9) {
          uVar9 = uVar11;
        }
        uVar10 = uVar7 + 3 + uVar6 + uVar11;
        if (uVar10 <= uVar11 && uVar11 - uVar10 != 0) {
          p_Var4 = FUN_005516c1();
          *(undefined4 *)p_Var4 = 0xc;
          goto LAB_0055511f;
        }
        if ((uint *)(uVar6 + 3 + uVar7 + (int)pvVar1) < param_1) {
          local_18 = (LPCVOID)FUN_0054a550(uVar10);
LAB_005551cd:
          if (local_18 == (LPCVOID)0x0) goto LAB_0055511f;
          bVar3 = true;
          local_8 = pvVar1;
        }
        else {
          p_Var4 = FUN_005516c1();
          uVar2 = *(undefined4 *)p_Var4;
          local_18 = (LPCVOID)FUN_0055f720(pvVar1,uVar10);
          if (local_18 == (LPCVOID)0x0) {
            p_Var4 = FUN_005516c1();
            *(undefined4 *)p_Var4 = uVar2;
            local_18 = (LPCVOID)FUN_0054a550(uVar10);
            goto LAB_005551cd;
          }
          bVar3 = false;
          local_8 = local_18;
        }
        if ((local_18 != pvVar1) || ((-uVar6 & (int)param_1 + param_5 + uVar7) != 0)) {
          param_1 = (uint *)(((int)local_18 + param_5 + 3 + uVar7 + uVar6 & -uVar6) - param_5);
          FUN_00543e20(param_1,(uint *)((int)local_8 + iVar8),uVar9);
          if (bVar3) {
            thunk_FUN_00557ba0(local_8);
          }
          iVar8 = (int)param_1 - uVar7;
          goto LAB_005552fb;
        }
        goto LAB_00555316;
      }
      p_Var4 = FUN_005516c1();
      *(undefined4 *)p_Var4 = 0x16;
      FUN_005480bc();
    }
LAB_0055511f:
    param_1 = (uint *)0x0;
  }
  return param_1;
}

