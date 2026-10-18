/* Entry: 0x00513af0; symbol: FUN_00513af0; body bytes: 87 */

void __thiscall FUN_00513af0(void *this,int param_1)

{
  undefined4 *puVar1;
  int local_10 [2];
  void *local_8;
  
  *(int *)((int)this + 0x90) = *(int *)((int)this + 0x90) + param_1;
  local_10[1] = 999999;
  local_10[0] = 0;
  local_8 = this;
  puVar1 = (undefined4 *)_Find_unchecked<>((int *)((int)this + 0x90),local_10,local_10 + 1);
  *(undefined4 *)((int)local_8 + 0x90) = *puVar1;
  return;
}

