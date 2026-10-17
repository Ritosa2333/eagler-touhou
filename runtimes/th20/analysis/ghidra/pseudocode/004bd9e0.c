/* Entry: 0x004bd9e0; symbol: FUN_004bd9e0; body bytes: 78 */

void __thiscall FUN_004bd9e0(void *this,undefined4 param_1)

{
  undefined4 *puVar1;
  int local_10 [2];
  void *local_8;
  
  *(undefined4 *)((int)this + 0xd4) = param_1;
  local_10[1] = 10;
  local_10[0] = 2;
  local_8 = this;
  puVar1 = (undefined4 *)_Find_unchecked<>((int *)((int)this + 0xd4),local_10,local_10 + 1);
  *(undefined4 *)((int)local_8 + 0xd4) = *puVar1;
  return;
}

