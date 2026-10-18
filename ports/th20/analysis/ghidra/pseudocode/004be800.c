/* Entry: 0x004be800; symbol: FUN_004be800; body bytes: 69 */

void __thiscall FUN_004be800(void *this,undefined4 param_1)

{
  undefined4 *puVar1;
  int local_10 [2];
  void *local_8;
  
  *(undefined4 *)((int)this + 0x78) = param_1;
  local_10[1] = 4;
  local_10[0] = 0;
  local_8 = this;
  puVar1 = (undefined4 *)_Find_unchecked<>((int *)((int)this + 0x78),local_10,local_10 + 1);
  *(undefined4 *)((int)local_8 + 0x78) = *puVar1;
  return;
}

