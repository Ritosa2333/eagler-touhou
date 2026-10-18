/* Entry: 0x00477790; symbol: FUN_00477790; body bytes: 45 */

void __thiscall FUN_00477790(void *this,uint param_1)

{
  *(uint *)((int)this + 0x3344) = *(uint *)((int)this + 0x3344) & 0xffffffef | (param_1 & 1) << 4;
  return;
}

