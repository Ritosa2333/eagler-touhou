/* Entry: 0x00474d80; symbol: FUN_00474d80; body bytes: 73 */

undefined4 __fastcall FUN_00474d80(int param_1)

{
  undefined4 *puVar1;
  int local_10 [3];
  
  local_10[1] = 999;
  local_10[0] = 0;
  local_10[2] = param_1;
  puVar1 = (undefined4 *)_Find_unchecked<>((int *)(param_1 + 500),local_10,local_10 + 1);
  *(undefined4 *)(local_10[2] + 500) = *puVar1;
  return *(undefined4 *)(local_10[2] + 500);
}

