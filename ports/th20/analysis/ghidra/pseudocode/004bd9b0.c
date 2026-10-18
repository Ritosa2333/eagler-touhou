/* Entry: 0x004bd9b0; symbol: FUN_004bd9b0; body bytes: 48 */

void __thiscall FUN_004bd9b0(void *this,uint param_1)

{
  *(uint *)((int)this + 0xe8) = *(uint *)((int)this + 0xe8) & 0xff7fffff | (param_1 & 1) << 0x17;
  return;
}

