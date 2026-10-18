/* Entry: 0x004777c0; symbol: FUN_004777c0; body bytes: 47 */

void __thiscall FUN_004777c0(void *this,undefined4 param_1)

{
  *(undefined4 *)((int)this + 0x54) = param_1;
  *(uint *)((int)this + 0x49c) = *(uint *)((int)this + 0x49c) | 4;
  return;
}

