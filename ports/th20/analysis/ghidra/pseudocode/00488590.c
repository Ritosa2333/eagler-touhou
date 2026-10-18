/* Entry: 0x00488590; symbol: FUN_00488590; body bytes: 61 */

void __thiscall FUN_00488590(void *this,int param_1)

{
  if (*(int *)((int)this + param_1 * 4 + 200) < 99999) {
    *(int *)((int)this + param_1 * 4 + 200) = *(int *)((int)this + param_1 * 4 + 200) + 1;
  }
  return;
}

