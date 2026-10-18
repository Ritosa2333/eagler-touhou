/* Entry: 0x0049c360; symbol: FUN_0049c360; body bytes: 36 */

void __thiscall FUN_0049c360(void *this,uint param_1)

{
  *(uint *)((int)this + 0x18) = *(uint *)((int)this + 0x18) & 0xfffffffe | param_1 & 1;
  return;
}

