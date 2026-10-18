/* Entry: 0x00424f30; symbol: FUN_00424f30; body bytes: 105 */

void __cdecl FUN_00424f30(int param_1)

{
  undefined4 local_18;
  undefined4 local_14;
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_18 = 0;
  local_14 = 0;
  local_10 = (float)DAT_005b87f8;
  local_c = (float)DAT_005b87fc;
  FUN_00423650(&local_18,*(int *)(param_1 + 0x18) << 0x18 | *(uint *)(param_1 + 0x20));
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

