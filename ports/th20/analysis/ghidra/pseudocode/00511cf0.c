/* Entry: 0x00511cf0; symbol: FUN_00511cf0; body bytes: 477 */

float10 __fastcall FUN_00511cf0(float *param_1)

{
  float10 fVar1;
  float fVar2;
  
  if (param_1[10] == 9.80909e-45) {
    *param_1 = *param_1 + param_1[1];
    param_1[4] = *param_1;
  }
  else if (param_1[10] == 2.38221e-44) {
    *param_1 = *param_1 + param_1[3];
    param_1[3] = param_1[3] + param_1[1];
    param_1[4] = *param_1;
  }
  else if (param_1[10] == 1.12104e-44) {
    fVar1 = FUN_00423bd0((int)(param_1 + 5));
    fVar2 = (float)fVar1 / (float)(int)param_1[9];
    param_1[4] = *param_1 *
                 (fVar2 - DAT_0056c8cc) * (fVar2 - DAT_0056c8cc) *
                 (DAT_0056c8d0 * fVar2 + DAT_0056c8cc) +
                 param_1[1] * fVar2 * fVar2 * (DAT_0056c8d8 - DAT_0056c8d0 * fVar2) +
                 param_1[2] * (DAT_0056c8cc - fVar2) * (DAT_0056c8cc - fVar2) * fVar2 +
                 param_1[3] * (fVar2 - DAT_0056c8cc) * fVar2 * fVar2;
  }
  else {
    fVar1 = (float10)FUN_00437540((int)param_1);
    param_1[4] = (param_1[1] - *param_1) * (float)fVar1 + *param_1;
  }
  return (float10)param_1[4];
}

