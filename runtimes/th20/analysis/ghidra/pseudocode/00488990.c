/* Entry: 0x00488990; symbol: FUN_00488990; body bytes: 105 */

void __thiscall FUN_00488990(void *this,int param_1,int param_2)

{
  *(int *)((int)this + 0xa8) = ((param_1 + 0x42) % 1000) * 100;
  *(int *)((int)this + 0xa8) = (param_2 + 0x21) % 100 + *(int *)((int)this + 0xa8);
  *(int *)((int)this + 0xa8) = (param_1 + 0x16 + param_2) * 100000 + *(int *)((int)this + 0xa8);
  return;
}

