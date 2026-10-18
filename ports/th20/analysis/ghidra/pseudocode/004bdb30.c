/* Entry: 0x004bdb30; symbol: FUN_004bdb30; body bytes: 78 */

void __thiscall FUN_004bdb30(void *this,undefined4 param_1)

{
  undefined4 *puVar1;
  int local_10 [2];
  void *local_8;
  
  *(undefined4 *)((int)this + 0x1e8) = param_1;
  local_10[1] = 9;
  local_10[0] = 0;
  local_8 = this;
  puVar1 = (undefined4 *)_Find_unchecked<>((int *)((int)this + 0x1e8),local_10,local_10 + 1);
  *(undefined4 *)((int)local_8 + 0x1e8) = *puVar1;
  return;
}

