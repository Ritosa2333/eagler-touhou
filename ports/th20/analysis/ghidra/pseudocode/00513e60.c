/* Entry: 0x00513e60; symbol: FUN_00513e60; body bytes: 64 */

undefined4 __fastcall FUN_00513e60(int param_1)

{
  undefined4 *puVar1;
  int local_10 [3];
  
  local_10[1] = 5000;
  local_10[0] = 0;
  local_10[2] = param_1;
  puVar1 = (undefined4 *)_Find_unchecked<>((int *)(param_1 + 0x60),local_10,local_10 + 1);
  *(undefined4 *)(local_10[2] + 0x60) = *puVar1;
  return *(undefined4 *)(local_10[2] + 0x60);
}

