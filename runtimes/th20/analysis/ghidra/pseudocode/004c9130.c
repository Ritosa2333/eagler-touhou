/* Entry: 0x004c9130; symbol: FUN_004c9130; body bytes: 49 */

float * __thiscall FUN_004c9130(void *this,float *param_1)

{
  float10 fVar1;
  
                    /* WARNING: Load size is inaccurate */
  fVar1 = FUN_00438540(*this + *param_1);
  *(float *)this = (float)fVar1;
  return (float *)this;
}

