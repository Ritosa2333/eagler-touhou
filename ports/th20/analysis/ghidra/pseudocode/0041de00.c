/* Entry: 0x0041de00; symbol: FUN_0041de00; body bytes: 44 */

void __thiscall FUN_0041de00(void *this,uint param_1)

{
  *(uint *)((int)this + 0x2090) = *(uint *)((int)this + 0x2090) & 0xfffffffd | (param_1 & 1) << 1;
  return;
}

