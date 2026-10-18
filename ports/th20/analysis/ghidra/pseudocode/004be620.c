/* Entry: 0x004be620; symbol: FUN_004be620; body bytes: 69 */

void __thiscall FUN_004be620(void *this,undefined4 param_1)

{
  undefined4 *puVar1;
  int local_10 [2];
  void *local_8;
  
  *(undefined4 *)((int)this + 0x5c) = param_1;
  local_10[1] = 10000;
  local_10[0] = 0;
  local_8 = this;
  puVar1 = (undefined4 *)_Find_unchecked<>((int *)((int)this + 0x5c),local_10,local_10 + 1);
  *(undefined4 *)((int)local_8 + 0x5c) = *puVar1;
  return;
}

