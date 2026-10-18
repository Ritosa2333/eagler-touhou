/* Entry: 0x00561559; symbol: FUN_00561559; body bytes: 163 */

void FUN_00561559(ushort *param_1)

{
  uint *puVar1;
  int iVar2;
  int iVar3;
  LCID Locale;
  LCTYPE LCType;
  WCHAR local_f8 [120];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar2 = FUN_005579c2();
  iVar3 = FUN_005579c2();
  puVar1 = *(uint **)(iVar3 + 0x34c);
  Locale = FUN_00561a92(param_1);
  LCType = 0x1002;
  if (*(int *)(iVar2 + 100) != 0) {
    LCType = 7;
  }
  iVar3 = GetLocaleInfoW(Locale,LCType,local_f8,0x78);
  if (iVar3 == 0) {
    *puVar1 = 0;
  }
  else {
    iVar2 = FUN_0055df10(*(ushort **)(iVar2 + 0x54),(ushort *)local_f8);
    if ((iVar2 == 0) && (iVar2 = _TestDefaultCountry((short)Locale), iVar2 != 0)) {
      *puVar1 = *puVar1 | 4;
      puVar1[2] = Locale;
      puVar1[1] = Locale;
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

