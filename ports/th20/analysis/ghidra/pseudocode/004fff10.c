/* Entry: 0x004fff10; symbol: FUN_004fff10; body bytes: 75 */

uint __thiscall FUN_004fff10(void *this,uint param_1)

{
  FUN_00423520((void *)((int)this + 0x2070),0);
  *(uint *)((int)this + 0x14) = *(uint *)((int)this + 0x14) & 0xffffff3f | (param_1 & 3) << 6;
  return param_1 & 3;
}

