/* Entry: 0x00514170; symbol: FUN_00514170; body bytes: 78 */

void __thiscall FUN_00514170(void *this,undefined4 param_1)

{
  undefined4 *puVar1;
  int local_10 [2];
  void *local_8;
  
  *(undefined4 *)((int)this + 0x98) = param_1;
  local_10[1] = 4;
  local_10[0] = -1;
  local_8 = this;
  puVar1 = (undefined4 *)_Find_unchecked<>((int *)((int)this + 0x98),local_10,local_10 + 1);
  *(undefined4 *)((int)local_8 + 0x98) = *puVar1;
  return;
}

