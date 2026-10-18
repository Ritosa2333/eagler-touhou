/* Entry: 0x00477ff0; symbol: FUN_00477ff0; body bytes: 73 */

undefined4 __fastcall FUN_00477ff0(int param_1)

{
  undefined4 *puVar1;
  int local_10 [3];
  
  local_10[1] = 10;
  local_10[0] = 0;
  local_10[2] = param_1;
  puVar1 = (undefined4 *)_Find_unchecked<>((int *)(param_1 + 0xcc),local_10,local_10 + 1);
  *(undefined4 *)(local_10[2] + 0xcc) = *puVar1;
  return *(undefined4 *)(local_10[2] + 0xcc);
}

