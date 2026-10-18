/* Entry: 0x00513ee0; symbol: FUN_00513ee0; body bytes: 64 */

undefined4 __fastcall FUN_00513ee0(int param_1)

{
  undefined4 *puVar1;
  int local_10 [3];
  
  local_10[1] = 1000;
  local_10[0] = 0;
  local_10[2] = param_1;
  puVar1 = (undefined4 *)_Find_unchecked<>((int *)(param_1 + 0x6c),local_10,local_10 + 1);
  *(undefined4 *)(local_10[2] + 0x6c) = *puVar1;
  return *(undefined4 *)(local_10[2] + 0x6c);
}

