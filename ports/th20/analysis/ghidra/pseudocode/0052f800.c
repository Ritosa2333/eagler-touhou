/* Entry: 0x0052f800; symbol: FUN_0052f800; body bytes: 58 */

void __thiscall FUN_0052f800(void *this,undefined4 param_1)

{
  *(undefined4 *)((int)this + 0x2c) = *(undefined4 *)((int)this + 0x30);
  *(undefined4 *)((int)this + 0x30) = param_1;
  *(undefined4 *)((int)this + 0x34) = 0;
  FUN_00423520((void *)((int)this + 0x38),0);
  return;
}

