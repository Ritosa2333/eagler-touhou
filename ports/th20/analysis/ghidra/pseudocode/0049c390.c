/* Entry: 0x0049c390; symbol: FUN_0049c390; body bytes: 42 */

void __thiscall FUN_0049c390(void *this,uint param_1)

{
  *(uint *)((int)this + 0x14) = *(uint *)((int)this + 0x14) & 0xfffffbff | (param_1 & 1) << 10;
  return;
}

