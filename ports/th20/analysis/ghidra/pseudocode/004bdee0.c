/* Entry: 0x004bdee0; symbol: FUN_004bdee0; body bytes: 78 */

void __thiscall FUN_004bdee0(void *this,undefined4 param_1)

{
  undefined4 *puVar1;
  int local_10 [2];
  void *local_8;
  
  *(undefined4 *)((int)this + 0xbc) = param_1;
  local_10[1] = 7;
  local_10[0] = 0;
  local_8 = this;
  puVar1 = (undefined4 *)_Find_unchecked<>((int *)((int)this + 0xbc),local_10,local_10 + 1);
  *(undefined4 *)((int)local_8 + 0xbc) = *puVar1;
  return;
}

