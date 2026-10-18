/* Entry: 0x0052cda0; symbol: FUN_0052cda0; body bytes: 39 */

void __thiscall FUN_0052cda0(void *this,uint param_1)

{
  *(uint *)((int)this + 0x6c) = *(uint *)((int)this + 0x6c) & 0xffffffdf | (param_1 & 1) << 5;
  return;
}

