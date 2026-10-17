/* Entry: 0x004d7d20; symbol: FUN_004d7d20; body bytes: 42 */

void __thiscall FUN_004d7d20(void *this,uint param_1)

{
  *(uint *)((int)this + 0x730) = *(uint *)((int)this + 0x730) & 0xfffffffe | param_1 & 1;
  return;
}

