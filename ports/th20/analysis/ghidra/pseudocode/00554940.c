/* Entry: 0x00554940; symbol: FUN_00554940; body bytes: 396 */

uint * __cdecl FUN_00554940(uint *param_1,uint param_2,uint param_3)

{
  LPCVOID pvVar1;
  undefined4 uVar2;
  SIZE_T SVar3;
  __acrt_ptd *p_Var4;
  LPCVOID pvVar5;
  LPCVOID pvVar6;
  uint uVar7;
  int iVar8;
  uint uVar9;
  bool bVar10;
  uint unaff_ESI;
  uint *puVar11;
  uint uVar12;
  uint uVar13;
  
  bVar10 = false;
  if (param_1 == (uint *)0x0) {
    if ((param_3 != 0) && ((param_3 & param_3 - 1) == 0)) {
      uVar12 = 4;
      if (4 < param_3) {
        uVar12 = param_3;
      }
      uVar7 = param_2 + 3 + uVar12;
      if (param_2 <= uVar7) {
        uVar7 = FUN_0054a550(uVar7);
        if (uVar7 == 0) {
          return (uint *)0x0;
        }
        puVar11 = (uint *)(uVar7 + 3 + uVar12 & -uVar12);
        puVar11[-1] = uVar7;
        return puVar11;
      }
LAB_00554a11:
      p_Var4 = FUN_005516c1();
      *(undefined4 *)p_Var4 = 0xc;
      return (uint *)0x0;
    }
LAB_00554ab5:
    p_Var4 = FUN_005516c1();
    *(undefined4 *)p_Var4 = 0x16;
    FUN_005480bc();
    return (uint *)0x0;
  }
  if (param_2 == 0) {
    thunk_FUN_00557ba0(*(LPVOID *)(((uint)param_1 & 0xfffffffc) - 4));
    return (uint *)0x0;
  }
  if ((param_3 == 0) || ((param_3 & param_3 - 1) != 0)) goto LAB_00554ab5;
  uVar12 = 4;
  if (4 < param_3) {
    uVar12 = param_3;
  }
  uVar13 = uVar12 - 1;
  pvVar1 = *(LPCVOID *)(((uint)param_1 & 0xfffffffc) - 4);
  iVar8 = (int)param_1 - (int)pvVar1;
  SVar3 = __msize_base(pvVar1);
  uVar7 = (int)pvVar1 + (SVar3 - (int)param_1);
  if (param_2 < uVar7) {
    uVar7 = param_2;
  }
  uVar9 = param_2 + 4 + uVar13;
  if (uVar9 < param_2) goto LAB_00554a11;
  if (param_1 <= (uint *)((int)pvVar1 + uVar12 + 3)) {
    p_Var4 = FUN_005516c1();
    uVar2 = *(undefined4 *)p_Var4;
    pvVar5 = (LPCVOID)FUN_0055f720(pvVar1,uVar9);
    pvVar6 = pvVar5;
    if (pvVar5 != (LPCVOID)0x0) goto LAB_00554a73;
    p_Var4 = FUN_005516c1();
    *(undefined4 *)p_Var4 = uVar2;
    unaff_ESI = uVar9;
  }
  pvVar6 = (LPCVOID)FUN_0054a550(unaff_ESI);
  if (pvVar6 == (LPCVOID)0x0) {
    return (uint *)0x0;
  }
  bVar10 = true;
  pvVar5 = pvVar1;
LAB_00554a73:
  if ((pvVar6 != pvVar1) || (((uint)param_1 & ~uVar13) != 0)) {
    param_1 = (uint *)(uVar12 + 3 + (int)pvVar6 & ~uVar13);
    FUN_00543e20(param_1,(uint *)(iVar8 + (int)pvVar5),uVar7);
    if (bVar10) {
      thunk_FUN_00557ba0(pvVar5);
    }
    param_1[-1] = (uint)pvVar6;
  }
  return param_1;
}

