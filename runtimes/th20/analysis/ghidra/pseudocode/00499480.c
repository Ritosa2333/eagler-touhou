/* Entry: 0x00499480; symbol: FUN_00499480; body bytes: 73 */

undefined4 __fastcall FUN_00499480(int param_1)

{
  undefined4 *puVar1;
  int local_10 [3];
  
  local_10[1] = 9999;
  local_10[0] = -1;
  local_10[2] = param_1;
  puVar1 = (undefined4 *)_Find_unchecked<>((int *)(param_1 + 0x204),local_10,local_10 + 1);
  *(undefined4 *)(local_10[2] + 0x204) = *puVar1;
  return *(undefined4 *)(local_10[2] + 0x204);
}

