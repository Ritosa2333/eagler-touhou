/* Entry: 0x00423fe0; symbol: FUN_00423fe0; body bytes: 38 */

void __thiscall FUN_00423fe0(void *this,uint param_1)

{
  *(uint *)((int)this + 0xc) = *(uint *)((int)this + 0xc) & 0xfffffff9 | (param_1 & 3) << 1;
  return;
}

