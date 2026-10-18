/* Entry: 0x00470970; symbol: FUN_00470970; body bytes: 81 */

void __fastcall FUN_00470970(undefined4 param_1)

{
  uint uVar1;
  
  uVar1 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  move<>(param_1);
  FUN_0040bd90();
  FUN_005429ee(uVar1 ^ (uint)&stack0xfffffffc);
  return;
}

