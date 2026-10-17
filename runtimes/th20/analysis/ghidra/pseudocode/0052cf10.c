/* Entry: 0x0052cf10; symbol: FUN_0052cf10; body bytes: 39 */

void __thiscall FUN_0052cf10(void *this,uint param_1)

{
  *(uint *)((int)this + 0x6c) = *(uint *)((int)this + 0x6c) & 0xffffffbf | (param_1 & 1) << 6;
  return;
}

