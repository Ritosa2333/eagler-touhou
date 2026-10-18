/* Entry: 0x00466a20; symbol: FUN_00466a20; body bytes: 47 */

float * __thiscall FUN_00466a20(void *this,float param_1)

{
  float10 fVar1;
  
                    /* WARNING: Load size is inaccurate */
  fVar1 = FUN_00438540(*this * param_1);
  *(float *)this = (float)fVar1;
  return (float *)this;
}

