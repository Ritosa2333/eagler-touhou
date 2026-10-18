/* Entry: 0x004b7e70; symbol: FUN_004b7e70; body bytes: 73 */

undefined4 __fastcall FUN_004b7e70(int param_1)

{
  undefined4 *puVar1;
  int local_10 [3];
  
  local_10[1] = 10;
  local_10[0] = 0;
  local_10[2] = param_1;
  puVar1 = (undefined4 *)_Find_unchecked<>((int *)(param_1 + 0xd0),local_10,local_10 + 1);
  *(undefined4 *)(local_10[2] + 0xd0) = *puVar1;
  return *(undefined4 *)(local_10[2] + 0xd0);
}

