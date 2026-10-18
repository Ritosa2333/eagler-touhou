/* Entry: 0x004296e0; symbol: FUN_004296e0; body bytes: 85 */

float * __thiscall FUN_004296e0(void *this,float *param_1)

{
                    /* WARNING: Load size is inaccurate */
  *(float *)this = *this + *param_1;
  *(float *)((int)this + 4) = *(float *)((int)this + 4) + param_1[1];
  *(float *)((int)this + 8) = *(float *)((int)this + 8) + param_1[2];
  return (float *)this;
}

