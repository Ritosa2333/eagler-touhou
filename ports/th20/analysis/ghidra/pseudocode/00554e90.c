/* Entry: 0x00554e90; symbol: FUN_00554e90; body bytes: 513 */

uint * __cdecl FUN_00554e90(uint *param_1,uint param_2,uint param_3,uint param_4)

{
  LPCVOID pvVar1;
  undefined4 uVar2;
  int iVar3;
  uint uVar4;
  __acrt_ptd *p_Var5;
  int iVar6;
  SIZE_T SVar7;
  LPCVOID pvVar8;
  uint uVar9;
  uint uVar10;
  bool bVar11;
  uint unaff_ESI;
  uint *puVar12;
  uint uVar13;
  LPVOID local_c;
  
  bVar11 = false;
  if (param_1 == (uint *)0x0) {
    if (((param_3 == 0) || ((param_3 & param_3 - 1) != 0)) ||
       ((param_4 != 0 && (param_2 <= param_4)))) {
      p_Var5 = FUN_005516c1();
      *(undefined4 *)p_Var5 = 0x16;
      FUN_005480bc();
      return (uint *)0x0;
    }
    uVar13 = 4;
    if (4 < param_3) {
      uVar13 = param_3;
    }
    iVar3 = (-param_4 & 3) + 3 + uVar13;
    uVar4 = iVar3 + param_2;
    if (param_2 <= uVar4) {
      iVar6 = FUN_0054a550(uVar4);
      if (iVar6 != 0) {
        puVar12 = (uint *)((iVar3 + iVar6 + param_4 & -uVar13) - param_4);
        *(int *)((int)puVar12 + (-4 - (-param_4 & 3))) = iVar6;
        return puVar12;
      }
      return (uint *)0x0;
    }
    p_Var5 = FUN_005516c1();
    *(undefined4 *)p_Var5 = 0xc;
    return (uint *)0x0;
  }
  if (param_2 == 0) {
    thunk_FUN_00557ba0(*(LPVOID *)(((uint)param_1 & 0xfffffffc) - 4));
  }
  else {
    if (((param_3 == 0) || ((param_3 & param_3 - 1) != 0)) ||
       ((param_4 != 0 && (param_2 <= param_4)))) {
      p_Var5 = FUN_005516c1();
      *(undefined4 *)p_Var5 = 0x16;
      FUN_005480bc();
      return (uint *)0x0;
    }
    uVar13 = 4;
    if (4 < param_3) {
      uVar13 = param_3;
    }
    uVar9 = -param_4 & 3;
    pvVar1 = *(LPCVOID *)(((uint)param_1 & 0xfffffffc) - 4);
    SVar7 = __msize_base(pvVar1);
    uVar4 = (int)pvVar1 + (SVar7 - (int)param_1);
    if (param_2 < uVar4) {
      uVar4 = param_2;
    }
    uVar10 = uVar9 + 3 + uVar13 + param_2;
    if (uVar10 < param_2) {
      p_Var5 = FUN_005516c1();
      *(undefined4 *)p_Var5 = 0xc;
      return (uint *)0x0;
    }
    if (param_1 <= (uint *)(uVar9 + 3 + uVar13 + (int)pvVar1)) {
      p_Var5 = FUN_005516c1();
      uVar2 = *(undefined4 *)p_Var5;
      pvVar8 = (LPCVOID)FUN_0055f720(pvVar1,uVar10);
      local_c = pvVar8;
      if (pvVar8 != (LPCVOID)0x0) goto LAB_00555021;
      p_Var5 = FUN_005516c1();
      *(undefined4 *)p_Var5 = uVar2;
      unaff_ESI = uVar10;
    }
    pvVar8 = (LPCVOID)FUN_0054a550(unaff_ESI);
    if (pvVar8 != (LPCVOID)0x0) {
      bVar11 = true;
      local_c = pvVar1;
LAB_00555021:
      if ((pvVar8 == pvVar1) && ((-uVar13 & (int)param_1 + param_4 + uVar9) == 0)) {
        return param_1;
      }
      puVar12 = (uint *)(((int)pvVar8 + param_4 + 3 + uVar9 + uVar13 & -uVar13) - param_4);
      FUN_00543e20(puVar12,(uint *)(((int)param_1 - (int)pvVar1) + (int)local_c),uVar4);
      if (bVar11) {
        thunk_FUN_00557ba0(local_c);
      }
      *(LPCVOID *)((int)puVar12 + (-4 - uVar9)) = pvVar8;
      return puVar12;
    }
  }
  return (uint *)0x0;
}

