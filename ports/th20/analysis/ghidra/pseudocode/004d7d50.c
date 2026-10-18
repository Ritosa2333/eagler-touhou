/* Entry: 0x004d7d50; symbol: FUN_004d7d50; body bytes: 38 */

void __thiscall FUN_004d7d50(void *this,undefined4 *param_1)

{
  *(undefined4 *)((int)this + 0x58) = *param_1;
  *(undefined4 *)((int)this + 0x5c) = param_1[1];
  *(undefined4 *)((int)this + 0x60) = param_1[2];
  return;
}

