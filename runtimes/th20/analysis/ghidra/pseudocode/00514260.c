/* Entry: 0x00514260; symbol: FUN_00514260; body bytes: 75 */

void __thiscall FUN_00514260(void *this,int param_1)

{
  undefined4 *puVar1;
  int local_10 [2];
  void *local_8;
  
  *(int *)((int)this + 0x5c) = *(int *)((int)this + 0x5c) - param_1;
  local_10[1] = 10000;
  local_10[0] = 0;
  local_8 = this;
  puVar1 = (undefined4 *)_Find_unchecked<>((int *)((int)this + 0x5c),local_10,local_10 + 1);
  *(undefined4 *)((int)local_8 + 0x5c) = *puVar1;
  return;
}

