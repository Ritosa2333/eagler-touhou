/* Entry: 0x004ffc80; symbol: FUN_004ffc80; body bytes: 41 */

void __thiscall FUN_004ffc80(void *this,int param_1)

{
                    /* WARNING: Load size is inaccurate */
  *(int *)this = *this + param_1;
  *(int *)((int)this + 4) = *(int *)((int)this + 4) - param_1;
  return;
}

