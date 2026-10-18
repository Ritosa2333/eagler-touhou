/* Entry: 0x00439960; symbol: FUN_00439960; body bytes: 77 */

float * __thiscall FUN_00439960(void *this,float *param_1,float *param_2)

{
  float fVar1;
  
                    /* WARNING: Load size is inaccurate */
  *(float *)this = *this + *param_2;
  *(float *)((int)this + 4) = *(float *)((int)this + 4) + param_2[1];
                    /* WARNING: Load size is inaccurate */
  fVar1 = *(float *)((int)this + 4);
  *param_1 = *this;
  param_1[1] = fVar1;
  return param_1;
}

