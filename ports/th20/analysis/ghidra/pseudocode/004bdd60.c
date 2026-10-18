/* Entry: 0x004bdd60; symbol: FUN_004bdd60; body bytes: 38 */

void __thiscall FUN_004bdd60(void *this,uint param_1)

{
  *(uint *)((int)this + 0x6c) = *(uint *)((int)this + 0x6c) & 0xfffffffd | (param_1 & 1) << 1;
  return;
}

