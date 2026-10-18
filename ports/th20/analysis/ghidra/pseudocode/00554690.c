/* Entry: 0x00554690; symbol: FUN_00554690; body bytes: 672 */

uint * __cdecl FUN_00554690(uint *param_1,uint param_2,uint param_3,uint param_4,uint param_5)

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
  uVar9 = param_2 * param_3;
  if (param_1 == (uint *)0x0) {
    uVar12 = 0;
    if (((param_4 == 0) || ((param_4 & param_4 - 1) != 0)) || ((param_5 != 0 && (uVar9 <= param_5)))
       ) {
      p_Var4 = FUN_005516c1();
      *(undefined4 *)p_Var4 = 0x16;
      FUN_005480bc();
LAB_0055490a:
      param_1 = (uint *)0x0;
    }
    else {
      uVar6 = 4;
      if (4 < param_4) {
        uVar6 = param_4;
      }
      iVar8 = (-param_5 & 3) + 3 + uVar6;
      uVar11 = iVar8 + uVar9;
      if (uVar11 <= uVar9 && uVar9 - uVar11 != 0) {
        p_Var4 = FUN_005516c1();
        *(undefined4 *)p_Var4 = 0xc;
        goto LAB_0055490a;
      }
      local_18 = (LPCVOID)FUN_0054a550(uVar11);
      if (local_18 == (LPCVOID)0x0) goto LAB_0055490a;
      param_1 = (uint *)(((int)local_18 + param_5 + iVar8 & -uVar6) - param_5);
      iVar8 = (int)param_1 - (-param_5 & 3);
LAB_005548f5:
      *(LPCVOID *)(iVar8 + -4) = local_18;
    }
    if (param_1 == (uint *)0x0) {
      return (uint *)0x0;
    }
LAB_00554910:
    if (uVar12 < uVar9) {
      _memset((void *)((int)param_1 + uVar12),0,uVar9 - uVar12);
    }
  }
  else {
    SVar5 = __msize_base(*(LPCVOID *)(((uint)param_1 & 0xfffffffc) - 4));
    uVar12 = 4;
    if (4 < param_4) {
      uVar12 = param_4;
    }
    uVar12 = SVar5 + ((-3 - uVar12) - (-param_5 & 3));
    if (uVar9 == 0) {
      thunk_FUN_00557ba0(*(LPVOID *)(((uint)param_1 & 0xfffffffc) - 4));
    }
    else {
      if (((param_4 != 0) && ((param_4 & param_4 - 1) == 0)) &&
         ((param_5 == 0 || (param_5 < uVar9)))) {
        pvVar1 = *(LPCVOID *)(((uint)param_1 & 0xfffffffc) - 4);
        uVar6 = 4;
        if (4 < param_4) {
          uVar6 = param_4;
        }
        uVar7 = -param_5 & 3;
        iVar8 = (int)param_1 - (int)pvVar1;
        SVar5 = __msize_base(pvVar1);
        uVar11 = (int)pvVar1 + (SVar5 - (int)param_1);
        if (uVar9 < uVar11) {
          uVar11 = uVar9;
        }
        uVar10 = uVar7 + 3 + uVar6 + uVar9;
        if (uVar10 < uVar9) {
          p_Var4 = FUN_005516c1();
          *(undefined4 *)p_Var4 = 0xc;
          goto LAB_00554715;
        }
        if ((uint *)((int)pvVar1 + uVar7 + uVar6 + 3) < param_1) {
          local_18 = (LPCVOID)FUN_0054a550(uVar10);
LAB_005547c9:
          if (local_18 == (LPCVOID)0x0) goto LAB_00554715;
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
            goto LAB_005547c9;
          }
          bVar3 = false;
          local_8 = local_18;
        }
        if ((local_18 != pvVar1) || ((-uVar6 & (int)param_1 + param_5 + uVar7) != 0)) {
          param_1 = (uint *)(((int)local_18 + param_5 + 3 + uVar7 + uVar6 & -uVar6) - param_5);
          FUN_00543e20(param_1,(uint *)((int)local_8 + iVar8),uVar11);
          if (bVar3) {
            thunk_FUN_00557ba0(local_8);
          }
          iVar8 = (int)param_1 - uVar7;
          goto LAB_005548f5;
        }
        goto LAB_00554910;
      }
      p_Var4 = FUN_005516c1();
      *(undefined4 *)p_Var4 = 0x16;
      FUN_005480bc();
    }
LAB_00554715:
    param_1 = (uint *)0x0;
  }
  return param_1;
}

