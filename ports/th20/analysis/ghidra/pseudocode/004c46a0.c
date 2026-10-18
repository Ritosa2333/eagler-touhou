/* Entry: 0x004c46a0; symbol: FUN_004c46a0; body bytes: 75 */

void __thiscall FUN_004c46a0(void *this,int param_1)

{
  undefined4 *puVar1;
  int local_10 [2];
  void *local_8;
  
  *(int *)((int)this + 0x3c) = *(int *)((int)this + 0x3c) + param_1;
  local_10[1] = 1000000;
  local_10[0] = 0;
  local_8 = this;
  puVar1 = (undefined4 *)_Find_unchecked<>((int *)((int)this + 0x3c),local_10,local_10 + 1);
  *(undefined4 *)((int)local_8 + 0x3c) = *puVar1;
  return;
}

