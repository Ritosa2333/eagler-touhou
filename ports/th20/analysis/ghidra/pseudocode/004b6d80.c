/* Entry: 0x004b6d80; symbol: FUN_004b6d80; body bytes: 30 */

uint __thiscall FUN_004b6d80(void *this,uint param_1)

{
  return param_1 >> 2 & *(int *)((int)this + 8) - 1U;
}

