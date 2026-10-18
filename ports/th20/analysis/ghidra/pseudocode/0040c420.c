/* Entry: 0x0040c420; symbol: FUN_0040c420; body bytes: 80 */

void __fastcall FUN_0040c420(undefined4 param_1)

{
  uint uVar1;
  
  uVar1 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  move<>(param_1);
  FUN_0040bd90();
  FUN_005429ee(uVar1 ^ (uint)&stack0xfffffffc);
  return;
}

