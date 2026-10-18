/* Entry: 0x004c1fc0; symbol: FUN_004c1fc0; body bytes: 45 */

void __thiscall FUN_004c1fc0(void *this,uint param_1)

{
  *(uint *)((int)this + 0x358) = *(uint *)((int)this + 0x358) & 0xffffffef | (param_1 & 1) << 4;
  return;
}

