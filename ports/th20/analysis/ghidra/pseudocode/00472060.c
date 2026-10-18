/* Entry: 0x00472060; symbol: FUN_00472060; body bytes: 47 */

float * __thiscall FUN_00472060(void *this,float param_1)

{
  float10 fVar1;
  
                    /* WARNING: Load size is inaccurate */
  fVar1 = FUN_00438540(*this - param_1);
  *(float *)this = (float)fVar1;
  return (float *)this;
}

