/* Entry: 0x00554480; symbol: FUN_00554480; body bytes: 508 */

uint * __cdecl FUN_00554480(uint *param_1,uint param_2,uint param_3,uint param_4)

{
  LPCVOID pvVar1;
  undefined4 uVar2;
  int iVar3;
  uint uVar4;
  SIZE_T SVar5;
  __acrt_ptd *p_Var6;
  uint uVar7;
  uint uVar8;
  bool bVar9;
  uint *puVar10;
  uint uVar11;
  LPCVOID local_14;
  LPVOID local_8;
  
  bVar9 = false;
  if (param_1 == (uint *)0x0) {
    if (((param_3 == 0) || ((param_3 & param_3 - 1) != 0)) ||
       ((param_4 != 0 && (param_2 <= param_4)))) {
LAB_00554665:
      p_Var6 = FUN_005516c1();
      *(undefined4 *)p_Var6 = 0x16;
      FUN_005480bc();
      return (uint *)0x0;
    }
    uVar11 = 4;
    if (4 < param_3) {
      uVar11 = param_3;
    }
    iVar3 = (-param_4 & 3) + 3 + uVar11;
    uVar4 = iVar3 + param_2;
    if (uVar4 < param_2) {
LAB_0055459c:
      p_Var6 = FUN_005516c1();
      *(undefined4 *)p_Var6 = 0xc;
      return (uint *)0x0;
    }
    local_14 = (LPCVOID)FUN_0054a550(uVar4);
    if (local_14 == (LPCVOID)0x0) {
      return (uint *)0x0;
    }
    puVar10 = (uint *)(((int)local_14 + param_4 + iVar3 & -uVar11) - param_4);
    iVar3 = (int)puVar10 - (-param_4 & 3);
    goto LAB_00554660;
  }
  if (param_2 == 0) {
    thunk_FUN_00557ba0(*(LPVOID *)(((uint)param_1 & 0xfffffffc) - 4));
    return (uint *)0x0;
  }
  if (((param_3 == 0) || ((param_3 & param_3 - 1) != 0)) || ((param_4 != 0 && (param_2 <= param_4)))
     ) goto LAB_00554665;
  uVar11 = 4;
  if (4 < param_3) {
    uVar11 = param_3;
  }
  pvVar1 = *(LPCVOID *)(((uint)param_1 & 0xfffffffc) - 4);
  uVar7 = -param_4 & 3;
  SVar5 = __msize_base(pvVar1);
  uVar4 = (int)pvVar1 + (SVar5 - (int)param_1);
  if (param_2 < uVar4) {
    uVar4 = param_2;
  }
  uVar8 = uVar11 + uVar7 + param_2 + 3;
  if (uVar8 < param_2) goto LAB_0055459c;
  if ((uint *)(uVar7 + 3 + uVar11 + (int)pvVar1) < param_1) {
    local_14 = (LPCVOID)FUN_0054a550(uVar8);
LAB_005545fb:
    if (local_14 == (LPCVOID)0x0) {
      return (uint *)0x0;
    }
    bVar9 = true;
    local_8 = pvVar1;
  }
  else {
    p_Var6 = FUN_005516c1();
    uVar2 = *(undefined4 *)p_Var6;
    local_14 = (LPCVOID)FUN_0055f720(pvVar1,uVar8);
    local_8 = local_14;
    if (local_14 == (LPCVOID)0x0) {
      p_Var6 = FUN_005516c1();
      *(undefined4 *)p_Var6 = uVar2;
      local_14 = (LPCVOID)FUN_0054a550(uVar8);
      goto LAB_005545fb;
    }
  }
  if ((local_14 == pvVar1) && ((-uVar11 & (int)param_1 + param_4 + uVar7) == 0)) {
    return param_1;
  }
  puVar10 = (uint *)(((int)local_14 + param_4 + 3 + uVar7 + uVar11 & -uVar11) - param_4);
  FUN_00543e20(puVar10,(uint *)(((int)param_1 - (int)pvVar1) + (int)local_8),uVar4);
  if (bVar9) {
    thunk_FUN_00557ba0(local_8);
  }
  iVar3 = (int)puVar10 - uVar7;
LAB_00554660:
  *(LPCVOID *)(iVar3 + -4) = local_14;
  return puVar10;
}

