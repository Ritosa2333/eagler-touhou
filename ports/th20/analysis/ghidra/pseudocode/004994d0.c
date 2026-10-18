/* Entry: 0x004994d0; symbol: FUN_004994d0; body bytes: 73 */

undefined4 __fastcall FUN_004994d0(int param_1)

{
  undefined4 *puVar1;
  int local_10 [3];
  
  local_10[1] = 999;
  local_10[0] = 0;
  local_10[2] = param_1;
  puVar1 = (undefined4 *)_Find_unchecked<>((int *)(param_1 + 0x1fc),local_10,local_10 + 1);
  *(undefined4 *)(local_10[2] + 0x1fc) = *puVar1;
  return *(undefined4 *)(local_10[2] + 0x1fc);
}

