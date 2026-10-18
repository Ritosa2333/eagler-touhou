/* Entry: 0x00488510; symbol: FUN_00488510; body bytes: 61 */

void __thiscall FUN_00488510(void *this,int param_1)

{
  if (*(int *)((int)this + param_1 * 4 + 0xc0) < 99999) {
    *(int *)((int)this + param_1 * 4 + 0xc0) = *(int *)((int)this + param_1 * 4 + 0xc0) + 1;
  }
  return;
}

