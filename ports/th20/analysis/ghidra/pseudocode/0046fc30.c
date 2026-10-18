/* Entry: 0x0046fc30; symbol: FUN_0046fc30; body bytes: 62 */

void FUN_0046fc30(void)

{
  uint local_10 [3];
  
  local_10[2] = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_10[0] = 0;
  local_10[1] = 0;
  FUN_00543e20(local_10,(uint *)&stack0x00000004,8);
  FUN_005429ee(local_10[2] ^ (uint)&stack0xfffffffc);
  return;
}

