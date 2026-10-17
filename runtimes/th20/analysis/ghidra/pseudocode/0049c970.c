/* Entry: 0x0049c970; symbol: FUN_0049c970; body bytes: 78 */

void __thiscall FUN_0049c970(void *this,undefined4 param_1)

{
  undefined4 *puVar1;
  int local_10 [2];
  void *local_8;
  
  *(undefined4 *)((int)this + 0x1fc) = param_1;
  local_10[1] = 999;
  local_10[0] = 0;
  local_8 = this;
  puVar1 = (undefined4 *)_Find_unchecked<>((int *)((int)this + 0x1fc),local_10,local_10 + 1);
  *(undefined4 *)((int)local_8 + 0x1fc) = *puVar1;
  return;
}

