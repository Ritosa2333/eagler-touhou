/* Entry: 0x004504f0; symbol: FUN_004504f0; body bytes: 47 */

void __thiscall FUN_004504f0(void *this,undefined4 param_1)

{
  *(undefined4 *)((int)this + 0x3c) = param_1;
  *(uint *)((int)this + 0x49c) = *(uint *)((int)this + 0x49c) | 2;
  return;
}

