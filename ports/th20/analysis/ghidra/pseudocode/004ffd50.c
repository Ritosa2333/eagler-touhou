/* Entry: 0x004ffd50; symbol: FUN_004ffd50; body bytes: 42 */

void __thiscall FUN_004ffd50(void *this,uint param_1)

{
  *(uint *)((int)this + 0x1255c) = *(uint *)((int)this + 0x1255c) & 0xfffffffe | param_1 & 1;
  return;
}

