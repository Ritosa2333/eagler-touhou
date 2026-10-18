/* Entry: 0x00478260; symbol: FUN_00478260; body bytes: 45 */

void __thiscall FUN_00478260(void *this,uint param_1)

{
  *(uint *)((int)this + 0x358) = *(uint *)((int)this + 0x358) & 0xffffffdf | (param_1 & 1) << 5;
  return;
}

