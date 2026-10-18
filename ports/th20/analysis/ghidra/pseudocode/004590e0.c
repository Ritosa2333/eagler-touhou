/* Entry: 0x004590e0; symbol: FUN_004590e0; body bytes: 172 */

void __thiscall FUN_004590e0(void *this,float *param_1,float *param_2,float param_3,int param_4)

{
  float fVar1;
  float fVar2;
  void *extraout_ECX;
  float10 fVar3;
  float10 fVar4;
  
  fVar3 = FUN_00439820(this,param_3);
  fVar4 = FUN_004397c0(extraout_ECX,param_3);
  for (; param_4 != 0; param_4 = param_4 + -1) {
    fVar1 = *param_2;
    fVar2 = param_2[1];
    param_1[1] = param_2[1] * (float)fVar4 + *param_2 * (float)fVar3;
    *param_1 = fVar1 * (float)fVar4 - fVar2 * (float)fVar3;
    param_1 = param_1 + 3;
    param_2 = param_2 + 3;
  }
  return;
}

