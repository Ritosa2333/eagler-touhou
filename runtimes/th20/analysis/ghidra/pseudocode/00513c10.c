/* Entry: 0x00513c10; symbol: FUN_00513c10; body bytes: 87 */

void __thiscall FUN_00513c10(void *this,int param_1)

{
  undefined4 *puVar1;
  int local_10 [2];
  void *local_8;
  
  *(int *)((int)this + 0x8c) = *(int *)((int)this + 0x8c) + param_1;
  local_10[1] = 999999;
  local_10[0] = 0;
  local_8 = this;
  puVar1 = (undefined4 *)_Find_unchecked<>((int *)((int)this + 0x8c),local_10,local_10 + 1);
  *(undefined4 *)((int)local_8 + 0x8c) = *puVar1;
  return;
}

