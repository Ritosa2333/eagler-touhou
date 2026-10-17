/* Entry: 0x004be5d0; symbol: FUN_004be5d0; body bytes: 78 */

void __thiscall FUN_004be5d0(void *this,undefined4 param_1)

{
  undefined4 *puVar1;
  int local_10 [2];
  void *local_8;
  
  *(undefined4 *)((int)this + 0x8c) = param_1;
  local_10[1] = 999999;
  local_10[0] = 0;
  local_8 = this;
  puVar1 = (undefined4 *)_Find_unchecked<>((int *)((int)this + 0x8c),local_10,local_10 + 1);
  *(undefined4 *)((int)local_8 + 0x8c) = *puVar1;
  return;
}

