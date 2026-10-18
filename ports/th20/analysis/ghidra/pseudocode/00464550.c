/* Entry: 0x00464550; symbol: FUN_00464550; body bytes: 44 */

void __thiscall FUN_00464550(void *this,byte param_1)

{
  *(byte *)((int)this + 0x4b0) = *(byte *)((int)this + 0x4b0) & 0xfd | (param_1 & 1) << 1;
  return;
}

