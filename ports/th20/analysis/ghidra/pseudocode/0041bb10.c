/* Entry: 0x0041bb10; symbol: FUN_0041bb10; body bytes: 94 */

void __cdecl FUN_0041bb10(uint *param_1)

{
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00543e20(&local_c,param_1,4);
  local_c = (local_c & 0xffffffdf) - 0x3a0041;
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

