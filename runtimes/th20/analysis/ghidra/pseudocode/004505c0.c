/* Entry: 0x004505c0; symbol: FUN_004505c0; body bytes: 47 */

void __thiscall FUN_004505c0(void *this,undefined4 param_1)

{
  *(undefined4 *)((int)this + 0x40) = param_1;
  *(uint *)((int)this + 0x49c) = *(uint *)((int)this + 0x49c) | 2;
  return;
}

