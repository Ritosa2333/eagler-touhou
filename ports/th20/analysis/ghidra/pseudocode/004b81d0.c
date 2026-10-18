/* Entry: 0x004b81d0; symbol: FUN_004b81d0; body bytes: 64 */

undefined4 __fastcall FUN_004b81d0(int param_1)

{
  undefined4 *puVar1;
  int local_10 [3];
  
  local_10[1] = 400;
  local_10[0] = 100;
  local_10[2] = param_1;
  puVar1 = (undefined4 *)_Find_unchecked<>((int *)(param_1 + 0x38),local_10,local_10 + 1);
  *(undefined4 *)(local_10[2] + 0x38) = *puVar1;
  return *(undefined4 *)(local_10[2] + 0x38);
}

