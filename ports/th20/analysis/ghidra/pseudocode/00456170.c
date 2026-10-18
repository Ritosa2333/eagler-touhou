/* Entry: 0x00456170; symbol: FUN_00456170; body bytes: 156 */

bool __cdecl
FUN_00456170(float *param_1,float *param_2,float param_3,float param_4,float param_5,float param_6)

{
  float fVar1;
  float10 fVar2;
  
  fVar2 = FUN_00445680(param_5 - param_3);
  fVar1 = DAT_0056e1a0;
  if (DAT_0056e1a0 <= (float)fVar2) {
    *param_1 = (param_6 - param_4) / (param_5 - param_3);
    *param_2 = param_4 - ((param_6 - param_4) * param_3) / (param_5 - param_3);
  }
  else {
    *param_1 = 0.0;
    *param_2 = param_3;
  }
  return (float)fVar2 < fVar1;
}

