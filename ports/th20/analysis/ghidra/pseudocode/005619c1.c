/* Entry: 0x005619c1; symbol: FUN_005619c1; body bytes: 209 */

void FUN_005619c1(ushort *param_1)

{
  uint *puVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  LCID Locale;
  undefined3 extraout_var;
  LCTYPE LCType;
  WCHAR local_f8 [120];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar3 = FUN_005579c2();
  iVar4 = FUN_005579c2();
  puVar1 = *(uint **)(iVar4 + 0x34c);
  Locale = FUN_00561a92(param_1);
  LCType = 0x1001;
  if (*(int *)(iVar3 + 0x60) != 0) {
    LCType = 3;
  }
  iVar4 = GetLocaleInfoW(Locale,LCType,local_f8,0x78);
  if (iVar4 == 0) {
    *puVar1 = 0;
    goto LAB_00561a81;
  }
  iVar4 = FUN_0055df10(*(ushort **)(iVar3 + 0x50),(ushort *)local_f8);
  if (iVar4 == 0) {
    if (*(int *)(iVar3 + 0x60) == 0) {
      iVar3 = 1;
      goto LAB_00561a61;
    }
  }
  else {
    if (((*(int *)(iVar3 + 0x60) != 0) || (*(int *)(iVar3 + 0x5c) == 0)) ||
       (iVar3 = FUN_0055df10(*(ushort **)(iVar3 + 0x50),(ushort *)local_f8), iVar3 != 0))
    goto LAB_00561a81;
    iVar3 = 0;
LAB_00561a61:
    bVar2 = FUN_00561bec(Locale,iVar3);
    if (CONCAT31(extraout_var,bVar2) == 0) goto LAB_00561a81;
  }
  *puVar1 = *puVar1 | 4;
  puVar1[1] = Locale;
  puVar1[2] = Locale;
LAB_00561a81:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

