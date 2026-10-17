/* Entry: 0x00497c60; symbol: FUN_00497c60; body bytes: 87 */

void __thiscall FUN_00497c60(void *this,int param_1)

{
  undefined4 *puVar1;
  int local_10 [2];
  void *local_8;
  
  *(int *)((int)this + 0xe0) = *(int *)((int)this + 0xe0) + param_1;
  local_10[1] = 999999;
  local_10[0] = 0;
  local_8 = this;
  puVar1 = (undefined4 *)_Find_unchecked<>((int *)((int)this + 0xe0),local_10,local_10 + 1);
  *(undefined4 *)((int)local_8 + 0xe0) = *puVar1;
  return;
}

