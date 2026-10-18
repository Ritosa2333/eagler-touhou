/* Entry: 0x004be940; symbol: FUN_004be940; body bytes: 69 */

void __thiscall FUN_004be940(void *this,undefined4 param_1)

{
  undefined4 *puVar1;
  int local_10 [2];
  void *local_8;
  
  *(undefined4 *)((int)this + 0x4c) = param_1;
  local_10[1] = 500;
  local_10[0] = 0;
  local_8 = this;
  puVar1 = (undefined4 *)_Find_unchecked<>((int *)((int)this + 0x4c),local_10,local_10 + 1);
  *(undefined4 *)((int)local_8 + 0x4c) = *puVar1;
  return;
}

