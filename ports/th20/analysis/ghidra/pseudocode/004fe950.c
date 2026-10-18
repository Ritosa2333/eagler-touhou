/* Entry: 0x004fe950; symbol: FUN_004fe950; body bytes: 87 */

void __thiscall FUN_004fe950(void *this,int param_1)

{
  undefined4 *puVar1;
  int local_10 [2];
  void *local_8;
  
  *(int *)((int)this + 200) = *(int *)((int)this + 200) + param_1;
  local_10[1] = 999;
  local_10[0] = 0;
  local_8 = this;
  puVar1 = (undefined4 *)_Find_unchecked<>((int *)((int)this + 200),local_10,local_10 + 1);
  *(undefined4 *)((int)local_8 + 200) = *puVar1;
  return;
}

