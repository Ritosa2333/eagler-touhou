/* Entry: 0x004864f0; symbol: FUN_004864f0; body bytes: 60 */

void __thiscall FUN_004864f0(void *this,undefined4 param_1,undefined4 param_2)

{
  *(undefined4 *)((int)this + 0x58) = param_1;
  *(undefined4 *)((int)this + 0x5c) = param_2;
  *(uint *)((int)this + 0x49c) = *(uint *)((int)this + 0x49c) | 4;
  return;
}

