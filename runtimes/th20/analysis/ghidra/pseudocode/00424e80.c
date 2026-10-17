/* Entry: 0x00424e80; symbol: FUN_00424e80; body bytes: 172 */

void __cdecl FUN_00424e80(int param_1)

{
  float local_18;
  float local_14;
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_18 = (DAT_0056cdac - DAT_0056cda4) / DAT_0056c8d0;
  local_14 = DAT_0056cd90;
  local_10 = (DAT_0056cdac - DAT_0056cda4) / DAT_0056c8d0 + DAT_0056cda4;
  local_c = DAT_0056cd90 + DAT_0056d7c0;
  FUN_00423650(&local_18,*(int *)(param_1 + 0x18) << 0x18 | *(uint *)(param_1 + 0x20));
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

