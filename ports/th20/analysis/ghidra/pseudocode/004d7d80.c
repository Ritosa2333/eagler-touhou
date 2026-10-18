/* Entry: 0x004d7d80; symbol: FUN_004d7d80; body bytes: 68 */

void __thiscall FUN_004d7d80(void *this,undefined4 *param_1)

{
  *(undefined4 *)((int)this + 0x58) = *param_1;
  *(undefined4 *)((int)this + 0x5c) = param_1[1];
  *(undefined4 *)((int)this + 0x60) = param_1[2];
  *(undefined4 *)((int)this + 0x6f8) = *(undefined4 *)((int)this + 0x58);
  *(undefined4 *)((int)this + 0x6fc) = *(undefined4 *)((int)this + 0x5c);
  *(undefined4 *)((int)this + 0x700) = *(undefined4 *)((int)this + 0x60);
  return;
}

