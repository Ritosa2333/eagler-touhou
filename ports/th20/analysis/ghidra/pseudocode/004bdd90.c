/* Entry: 0x004bdd90; symbol: FUN_004bdd90; body bytes: 39 */

void __thiscall FUN_004bdd90(void *this,uint param_1)

{
  *(uint *)((int)this + 0x6c) = *(uint *)((int)this + 0x6c) & 0xfffffffb | (param_1 & 1) << 2;
  return;
}

