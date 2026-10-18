/* Entry: 0x0040b5d0; symbol: FUN_0040b5d0; body bytes: 85 */

void __cdecl FUN_0040b5d0(undefined4 *param_1,undefined4 *param_2)

{
  undefined4 *puVar1;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  puVar1 = (undefined4 *)move<>(param_1);
  local_c = *puVar1;
  puVar1 = (undefined4 *)move<>(param_2);
  *param_1 = *puVar1;
  puVar1 = (undefined4 *)move<>(&local_c);
  *param_2 = *puVar1;
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

