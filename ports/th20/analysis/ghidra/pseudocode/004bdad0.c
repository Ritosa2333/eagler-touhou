/* Entry: 0x004bdad0; symbol: FUN_004bdad0; body bytes: 39 */

void __thiscall FUN_004bdad0(void *this,uint param_1)

{
  *(uint *)((int)this + 0x6c) = *(uint *)((int)this + 0x6c) & 0xffffffef | (param_1 & 1) << 4;
  return;
}

