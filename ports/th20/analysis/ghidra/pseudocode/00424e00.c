/* Entry: 0x00424e00; symbol: FUN_00424e00; body bytes: 128 */

void __cdecl FUN_00424e00(int param_1)

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
  FUN_004455c0(DAT_005c0028);
  FUN_0041dce0(&DAT_005c4d40,2);
  FUN_00423650(&local_18,*(int *)(param_1 + 0x18) << 0x18 | *(uint *)(param_1 + 0x20));
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

