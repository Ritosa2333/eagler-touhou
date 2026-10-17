/* Entry: 0x0051b920; symbol: FUN_0051b920; body bytes: 64 */

undefined4 __fastcall FUN_0051b920(int param_1)

{
  undefined4 *puVar1;
  int local_10 [3];
  
  local_10[1] = 500;
  local_10[0] = 100;
  local_10[2] = param_1;
  puVar1 = (undefined4 *)_Find_unchecked<>((int *)(param_1 + 0x50),local_10,local_10 + 1);
  *(undefined4 *)(local_10[2] + 0x50) = *puVar1;
  return *(undefined4 *)(local_10[2] + 0x50);
}

