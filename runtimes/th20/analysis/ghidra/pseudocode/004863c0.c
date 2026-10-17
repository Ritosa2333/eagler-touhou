/* Entry: 0x004863c0; symbol: FUN_004863c0; body bytes: 45 */

void __thiscall FUN_004863c0(void *this,byte param_1)

{
  *(byte *)((int)this + 0x4a1) = *(byte *)((int)this + 0x4a1) & 0xe3 | (param_1 & 7) << 2;
  return;
}

