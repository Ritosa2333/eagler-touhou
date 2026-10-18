/* Entry: 0x00488960; symbol: FUN_00488960; body bytes: 42 */

void __thiscall FUN_00488960(void *this,uint param_1)

{
  *(uint *)((int)this + 0x78) = *(uint *)((int)this + 0x78) & 0xfffffdff | (param_1 & 1) << 9;
  return;
}

