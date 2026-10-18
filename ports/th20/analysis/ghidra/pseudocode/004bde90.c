/* Entry: 0x004bde90; symbol: FUN_004bde90; body bytes: 78 */

void __thiscall FUN_004bde90(void *this,undefined4 param_1)

{
  undefined4 *puVar1;
  int local_10 [2];
  void *local_8;
  
  *(undefined4 *)((int)this + 0xb8) = param_1;
  local_10[1] = 7;
  local_10[0] = -1;
  local_8 = this;
  puVar1 = (undefined4 *)_Find_unchecked<>((int *)((int)this + 0xb8),local_10,local_10 + 1);
  *(undefined4 *)((int)local_8 + 0xb8) = *puVar1;
  return;
}

