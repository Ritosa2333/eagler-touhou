/* Entry: 0x00439910; symbol: FUN_00439910; body bytes: 72 */

float * __thiscall FUN_00439910(void *this,float *param_1,float param_2)

{
  float fVar1;
  
                    /* WARNING: Load size is inaccurate */
  *(float *)this = *this * param_2;
  *(float *)((int)this + 4) = *(float *)((int)this + 4) * param_2;
                    /* WARNING: Load size is inaccurate */
  fVar1 = *(float *)((int)this + 4);
  *param_1 = *this;
  param_1[1] = fVar1;
  return param_1;
}

