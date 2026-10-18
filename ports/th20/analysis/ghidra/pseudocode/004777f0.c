/* Entry: 0x004777f0; symbol: FUN_004777f0; body bytes: 47 */

void __thiscall FUN_004777f0(void *this,undefined4 param_1)

{
  *(undefined4 *)((int)this + 0x50) = param_1;
  *(uint *)((int)this + 0x49c) = *(uint *)((int)this + 0x49c) | 4;
  return;
}

