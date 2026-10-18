/* Entry: 0x00506cf0; symbol: FUN_00506cf0; body bytes: 39 */

void __thiscall FUN_00506cf0(void *this,uint param_1)

{
  *(uint *)((int)this + 0x14) = *(uint *)((int)this + 0x14) & 0xffffffbf | (param_1 & 1) << 6;
  return;
}

