/* Entry: 0x00439330; symbol: FUN_00439330; body bytes: 89 */

void __thiscall FUN_00439330(void *this,float *param_1,float param_2,float param_3)

{
  void *this_00;
  float10 fVar1;
  
  fVar1 = (float10)FUN_004371d0(this,param_2);
  *param_1 = (float)fVar1 * param_3;
  fVar1 = (float10)FUN_00439530(this_00,param_2);
  param_1[1] = (float)fVar1 * param_3;
  return;
}

