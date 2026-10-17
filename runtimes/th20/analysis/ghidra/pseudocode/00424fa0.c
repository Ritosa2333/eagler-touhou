/* Entry: 0x00424fa0; symbol: FUN_00424fa0; body bytes: 202 */

void __cdecl FUN_00424fa0(int param_1)

{
  int iVar1;
  int iVar2;
  float local_18;
  float local_14;
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar1 = FUN_0041cad0(0x5b6758);
  local_18 = (float)iVar1;
  iVar1 = FUN_0041caf0(0x5b6758);
  local_14 = (float)iVar1;
  iVar1 = FUN_0041cad0(0x5b6758);
  iVar2 = FUN_00424010(0x5b6758);
  local_10 = (float)iVar1 + (float)iVar2;
  iVar1 = FUN_0041caf0(0x5b6758);
  iVar2 = FUN_00421810(0x5b6758);
  local_c = (float)iVar1 + (float)iVar2;
  FUN_00423650(&local_18,*(int *)(param_1 + 0x18) << 0x18 | *(uint *)(param_1 + 0x24) & 0xffffff);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

