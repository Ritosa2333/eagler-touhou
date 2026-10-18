/* Entry: 0x004fee50; symbol: FUN_004fee50; body bytes: 63 */

void __thiscall FUN_004fee50(void *this,float *param_1)

{
  *(int *)this = (int)(*param_1 * DAT_0056cd98);
  *(int *)((int)this + 4) = (int)(param_1[1] * DAT_0056cd98);
  return;
}

