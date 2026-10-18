/* Entry: 0x004ffee0; symbol: FUN_004ffee0; body bytes: 40 */

void __thiscall FUN_004ffee0(void *this,undefined4 *param_1)

{
  *(undefined4 *)((int)this + 0x20f0) = *param_1;
  *(undefined4 *)((int)this + 0x20f4) = param_1[1];
  *(undefined4 *)((int)this + 0x20f8) = param_1[2];
  return;
}

