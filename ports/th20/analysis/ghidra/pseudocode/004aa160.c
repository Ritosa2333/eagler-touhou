/* Entry: 0x004aa160; symbol: FUN_004aa160; body bytes: 66 */

int __thiscall FUN_004aa160(void *this,uint param_1)

{
  if ((uint)((*(int *)((int)this + 8) - *(int *)((int)this + 4)) / 0x88) <= param_1) {
    FUN_004a9d70();
  }
  return param_1 * 0x88 + *(int *)((int)this + 4);
}

