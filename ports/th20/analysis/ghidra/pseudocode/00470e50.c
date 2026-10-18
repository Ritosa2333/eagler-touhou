/* Entry: 0x00470e50; symbol: FUN_00470e50; body bytes: 60 */

void __thiscall FUN_00470e50(void *this,undefined4 param_1,undefined4 param_2)

{
  *(undefined4 *)((int)this + 0x50) = param_1;
  *(undefined4 *)((int)this + 0x54) = param_2;
  *(uint *)((int)this + 0x49c) = *(uint *)((int)this + 0x49c) | 4;
  return;
}

