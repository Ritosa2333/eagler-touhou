/* Entry: 0x004be760; symbol: FUN_004be760; body bytes: 69 */

void __thiscall FUN_004be760(void *this,undefined4 param_1)

{
  undefined4 *puVar1;
  int local_10 [2];
  void *local_8;
  
  *(undefined4 *)((int)this + 100) = param_1;
  local_10[1] = 1000;
  local_10[0] = 0;
  local_8 = this;
  puVar1 = (undefined4 *)_Find_unchecked<>((int *)((int)this + 100),local_10,local_10 + 1);
  *(undefined4 *)((int)local_8 + 100) = *puVar1;
  return;
}

