/* Entry: 0x00459190; symbol: FUN_00459190; body bytes: 89 */

void __thiscall FUN_00459190(void *this,float *param_1,float param_2,float param_3,float param_4)

{
  void *this_00;
  float10 fVar1;
  
  fVar1 = (float10)FUN_004371d0(this,param_2);
  *param_1 = (float)fVar1 * param_3;
  fVar1 = (float10)FUN_00439530(this_00,param_2);
  param_1[1] = (float)fVar1 * param_4;
  return;
}

