/* Entry: 0x0049c4b0; symbol: FUN_0049c4b0; body bytes: 42 */

void __thiscall FUN_0049c4b0(void *this,uint param_1)

{
  *(uint *)((int)this + 0x94) = *(uint *)((int)this + 0x94) & 0xfffffffe | param_1 & 1;
  return;
}

