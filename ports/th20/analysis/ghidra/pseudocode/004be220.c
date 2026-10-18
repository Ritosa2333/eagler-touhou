/* Entry: 0x004be220; symbol: FUN_004be220; body bytes: 36 */

void __thiscall FUN_004be220(void *this,uint param_1)

{
  *(uint *)((int)this + 0x6c) = *(uint *)((int)this + 0x6c) & 0xfffffffe | param_1 & 1;
  return;
}

