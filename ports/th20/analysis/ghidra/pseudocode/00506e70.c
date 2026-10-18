/* Entry: 0x00506e70; symbol: FUN_00506e70; body bytes: 39 */

void __thiscall FUN_00506e70(void *this,uint param_1)

{
  *(uint *)((int)this + 0x44) = *(uint *)((int)this + 0x44) & 0xffffffdf | (param_1 & 1) << 5;
  return;
}

