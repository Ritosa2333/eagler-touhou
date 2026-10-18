/* Entry: 0x004530c0; symbol: FUN_004530c0; body bytes: 47 */

float * __thiscall FUN_004530c0(void *this,float param_1)

{
  float10 fVar1;
  
                    /* WARNING: Load size is inaccurate */
  fVar1 = FUN_00438540(*this + param_1);
  *(float *)this = (float)fVar1;
  return (float *)this;
}

