/* Entry: 0x005554e0; symbol: FUN_005554e0; body bytes: 564 */

uint * __cdecl FUN_005554e0(uint *param_1,uint param_2,uint param_3,uint param_4)

{
  LPCVOID pvVar1;
  undefined4 uVar2;
  bool bVar3;
  __acrt_ptd *p_Var4;
  uint uVar5;
  SIZE_T SVar6;
  uint uVar7;
  int iVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  LPCVOID local_14;
  LPVOID local_8;
  
  if ((param_2 != 0) && (0xffffffe0 / param_2 < param_3)) {
    p_Var4 = FUN_005516c1();
    *(undefined4 *)p_Var4 = 0xc;
    return (uint *)0x0;
  }
  uVar9 = param_2 * param_3;
  if (param_1 == (uint *)0x0) {
    uVar12 = 0;
    if ((param_4 == 0) || ((param_4 & param_4 - 1) != 0)) {
      p_Var4 = FUN_005516c1();
      *(undefined4 *)p_Var4 = 0x16;
      FUN_005480bc();
LAB_005556ee:
      param_1 = (uint *)0x0;
    }
    else {
      uVar5 = 4;
      if (4 < param_4) {
        uVar5 = param_4;
      }
      uVar10 = uVar9 + 3 + uVar5;
      if (uVar10 <= uVar9 && uVar9 - uVar10 != 0) {
        p_Var4 = FUN_005516c1();
        *(undefined4 *)p_Var4 = 0xc;
        goto LAB_005556ee;
      }
      local_14 = (LPCVOID)FUN_0054a550(uVar10);
      if (local_14 == (LPCVOID)0x0) goto LAB_005556ee;
      param_1 = (uint *)(uVar5 + 3 + (int)local_14 & -uVar5);
LAB_005556d9:
      param_1[-1] = (uint)local_14;
    }
    if (param_1 == (uint *)0x0) {
      return (uint *)0x0;
    }
LAB_005556f4:
    if (uVar12 < uVar9) {
      _memset((void *)((int)param_1 + uVar12),0,uVar9 - uVar12);
    }
  }
  else {
    uVar5 = (uint)param_1 & 0xfffffffc;
    SVar6 = __msize_base(*(LPCVOID *)(uVar5 - 4));
    uVar12 = 4;
    if (4 < param_4) {
      uVar12 = param_4;
    }
    uVar12 = (-3 - uVar12) + SVar6;
    if (uVar9 == 0) {
      thunk_FUN_00557ba0(*(LPVOID *)(uVar5 - 4));
    }
    else {
      if ((param_4 != 0) && ((param_4 & param_4 - 1) == 0)) {
        pvVar1 = *(LPCVOID *)(uVar5 - 4);
        uVar5 = 4;
        if (4 < param_4) {
          uVar5 = param_4;
        }
        uVar7 = uVar5 - 1;
        iVar8 = (int)param_1 - (int)pvVar1;
        SVar6 = __msize_base(pvVar1);
        uVar10 = (int)pvVar1 + (SVar6 - (int)param_1);
        if (uVar9 < uVar10) {
          uVar10 = uVar9;
        }
        uVar11 = uVar9 + 4 + uVar7;
        if (uVar11 < uVar9) {
          p_Var4 = FUN_005516c1();
          *(undefined4 *)p_Var4 = 0xc;
          goto LAB_00555558;
        }
        if ((uint *)((int)pvVar1 + uVar5 + 3) < param_1) {
          local_14 = (LPCVOID)FUN_0054a550(uVar11);
LAB_005555e3:
          if (local_14 == (LPCVOID)0x0) goto LAB_00555558;
          bVar3 = true;
          local_8 = pvVar1;
        }
        else {
          p_Var4 = FUN_005516c1();
          uVar2 = *(undefined4 *)p_Var4;
          local_14 = (LPCVOID)FUN_0055f720(pvVar1,uVar11);
          if (local_14 == (LPCVOID)0x0) {
            p_Var4 = FUN_005516c1();
            *(undefined4 *)p_Var4 = uVar2;
            local_14 = (LPCVOID)FUN_0054a550(uVar11);
            goto LAB_005555e3;
          }
          bVar3 = false;
          local_8 = local_14;
        }
        if ((local_14 != pvVar1) || (((uint)param_1 & ~uVar7) != 0)) {
          param_1 = (uint *)(uVar5 + 3 + (int)local_14 & ~uVar7);
          FUN_00543e20(param_1,(uint *)(iVar8 + (int)local_8),uVar10);
          if (bVar3) {
            thunk_FUN_00557ba0(local_8);
          }
          goto LAB_005556d9;
        }
        goto LAB_005556f4;
      }
      p_Var4 = FUN_005516c1();
      *(undefined4 *)p_Var4 = 0x16;
      FUN_005480bc();
    }
LAB_00555558:
    param_1 = (uint *)0x0;
  }
  return param_1;
}

