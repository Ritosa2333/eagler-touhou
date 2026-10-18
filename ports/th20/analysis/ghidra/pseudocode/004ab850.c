/* Entry: 0x004ab850; symbol: FUN_004ab850; body bytes: 39 */

void __thiscall FUN_004ab850(void *this,uint param_1)

{
  *(uint *)((int)this + 0x14) = *(uint *)((int)this + 0x14) & 0xffffffdf | (param_1 & 1) << 5;
  return;
}

