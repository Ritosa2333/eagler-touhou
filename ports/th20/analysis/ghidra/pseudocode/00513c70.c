/* Entry: 0x00513c70; symbol: FUN_00513c70; body bytes: 75 */

void __thiscall FUN_00513c70(void *this,int param_1)

{
  undefined4 *puVar1;
  int local_10 [2];
  void *local_8;
  
  *(int *)((int)this + 0x60) = *(int *)((int)this + 0x60) + param_1;
  local_10[1] = 5000;
  local_10[0] = 0;
  local_8 = this;
  puVar1 = (undefined4 *)_Find_unchecked<>((int *)((int)this + 0x60),local_10,local_10 + 1);
  *(undefined4 *)((int)local_8 + 0x60) = *puVar1;
  return;
}

