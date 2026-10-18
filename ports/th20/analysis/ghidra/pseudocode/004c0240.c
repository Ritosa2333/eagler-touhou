/* Entry: 0x004c0240; symbol: FUN_004c0240; body bytes: 77 */

float * __thiscall FUN_004c0240(void *this,float param_1)

{
                    /* WARNING: Load size is inaccurate */
  *(float *)this = *this / param_1;
  *(float *)((int)this + 4) = *(float *)((int)this + 4) / param_1;
  *(float *)((int)this + 8) = *(float *)((int)this + 8) / param_1;
  return (float *)this;
}

