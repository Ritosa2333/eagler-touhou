/* Entry: 0x004fe160; symbol: FUN_004fe160; body bytes: 58 */

void FUN_004fe160(void)

{
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_c = 0;
  FUN_00543e20(&local_c,(uint *)&stack0x00000004,4);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

