/* Entry: 0x004fe9b0; symbol: FUN_004fe9b0; body bytes: 75 */

void __thiscall FUN_004fe9b0(void *this,int param_1)

{
  undefined4 *puVar1;
  int local_10 [2];
  void *local_8;
  
  *(int *)((int)this + 0x78) = *(int *)((int)this + 0x78) + param_1;
  local_10[1] = 4;
  local_10[0] = 0;
  local_8 = this;
  puVar1 = (undefined4 *)_Find_unchecked<>((int *)((int)this + 0x78),local_10,local_10 + 1);
  *(undefined4 *)((int)local_8 + 0x78) = *puVar1;
  return;
}

