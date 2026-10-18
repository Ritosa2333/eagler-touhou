/* Entry: 0x004bd410; symbol: FUN_004bd410; body bytes: 73 */

undefined4 __fastcall FUN_004bd410(int param_1)

{
  undefined4 *puVar1;
  int local_10 [3];
  
  local_10[1] = 10;
  local_10[0] = 2;
  local_10[2] = param_1;
  puVar1 = (undefined4 *)_Find_unchecked<>((int *)(param_1 + 0xd4),local_10,local_10 + 1);
  *(undefined4 *)(local_10[2] + 0xd4) = *puVar1;
  return *(undefined4 *)(local_10[2] + 0xd4);
}

