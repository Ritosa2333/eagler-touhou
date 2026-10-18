/* Entry: 0x004dda30; symbol: FUN_004dda30; body bytes: 45 */

void __thiscall FUN_004dda30(void *this,byte param_1)

{
  *(byte *)((int)this + 0x4a0) = *(byte *)((int)this + 0x4a0) & 0xf3 | (param_1 & 3) << 2;
  return;
}

