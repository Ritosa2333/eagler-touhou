/* Entry: 0x0045cf10; symbol: FUN_0045cf10; body bytes: 202 */

void __cdecl FUN_0045cf10(float *param_1,float *param_2)

{
  float fVar1;
  float10 fVar2;
  
  fVar2 = FUN_00446b30(param_2,*param_2 * *param_2 + param_2[1] * param_2[1] +
                               param_2[2] * param_2[2]);
  fVar1 = (float)fVar2;
  fVar2 = FUN_00445680(fVar1);
  if (DAT_0056e1a0 <= (float)fVar2) {
    *param_1 = *param_2 / fVar1;
    param_1[1] = param_2[1] / fVar1;
    param_1[2] = param_2[2] / fVar1;
  }
  else {
    *param_1 = *param_2;
    param_1[1] = param_2[1];
    param_1[2] = param_2[2];
  }
  return;
}

