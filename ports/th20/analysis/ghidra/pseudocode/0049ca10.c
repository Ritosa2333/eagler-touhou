/* Entry: 0x0049ca10; symbol: FUN_0049ca10; body bytes: 42 */

void __thiscall FUN_0049ca10(void *this,uint param_1)

{
  *(uint *)((int)this + 0x78) = *(uint *)((int)this + 0x78) & 0xfffffeff | (param_1 & 1) << 8;
  return;
}

