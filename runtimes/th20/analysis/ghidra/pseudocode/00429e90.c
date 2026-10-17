/* Entry: 0x00429e90; symbol: FUN_00429e90; body bytes: 635 */

int __fastcall FUN_00429e90(int *param_1)

{
  bool bVar1;
  float10 fVar2;
  float fVar3;
  
  if (param_1[9] < 1) {
    if (param_1[9] == 0) {
      if ((param_1[10] != 7) && (param_1[10] != 0x11)) {
        return param_1[1];
      }
      return *param_1;
    }
  }
  else {
    FUN_00429400(param_1 + 5);
    bVar1 = FUN_004235c0(param_1 + 5,param_1[9]);
    if (bVar1) {
      FUN_00423520(param_1 + 5,param_1[9]);
      FUN_00429970((int)param_1);
      if ((param_1[10] != 7) && (param_1[10] != 0x11)) {
        return param_1[1];
      }
      return *param_1;
    }
  }
  if (param_1[10] == 7) {
    *param_1 = *param_1 + param_1[1];
    param_1[4] = *param_1;
  }
  else if (param_1[10] == 0x11) {
    *param_1 = *param_1 + param_1[3];
    param_1[3] = param_1[3] + param_1[1];
    param_1[4] = *param_1;
  }
  else if (param_1[10] == 8) {
    fVar2 = FUN_00423bd0((int)(param_1 + 5));
    fVar3 = (float)fVar2 / (float)param_1[9];
    param_1[4] = (int)((float)*param_1 *
                       (fVar3 - DAT_0056c8cc) * (fVar3 - DAT_0056c8cc) *
                       (DAT_0056c8d0 * fVar3 + DAT_0056c8cc) +
                       (float)param_1[1] * fVar3 * fVar3 * (DAT_0056c8d8 - DAT_0056c8d0 * fVar3) +
                       (float)param_1[2] * (DAT_0056c8cc - fVar3) * (DAT_0056c8cc - fVar3) * fVar3 +
                      (float)param_1[3] * (fVar3 - DAT_0056c8cc) * fVar3 * fVar3);
  }
  else {
    fVar2 = (float10)FUN_00437540((int)param_1);
    param_1[4] = (int)((float)(param_1[1] - *param_1) * (float)fVar2 + (float)*param_1);
  }
  return param_1[4];
}

