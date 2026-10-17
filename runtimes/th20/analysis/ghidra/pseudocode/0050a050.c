/* Entry: 0x0050a050; symbol: FUN_0050a050; body bytes: 73 */

undefined4 __fastcall FUN_0050a050(int param_1)

{
  undefined4 *puVar1;
  int local_10 [3];
  
  local_10[1] = 999999999;
  local_10[0] = 0;
  local_10[2] = param_1;
  puVar1 = (undefined4 *)_Find_unchecked<>((int *)(param_1 + 0x1f0),local_10,local_10 + 1);
  *(undefined4 *)(local_10[2] + 0x1f0) = *puVar1;
  return *(undefined4 *)(local_10[2] + 0x1f0);
}

