/* Entry: 0x00513710; symbol: FUN_00513710; body bytes: 666 */

byte __fastcall FUN_00513710(byte *param_1)

{
  bool bVar1;
  float10 fVar2;
  float fVar3;
  
  if (*(int *)(param_1 + 0x18) < 1) {
    if (*(int *)(param_1 + 0x18) == 0) {
      if ((*(int *)(param_1 + 0x1c) != 7) && (*(int *)(param_1 + 0x1c) != 0x11)) {
        return param_1[1];
      }
      return *param_1;
    }
  }
  else {
    FUN_00429400((undefined4 *)(param_1 + 8));
    bVar1 = FUN_004235c0(param_1 + 8,*(int *)(param_1 + 0x18));
    if (bVar1) {
      FUN_00423520(param_1 + 8,*(int *)(param_1 + 0x18));
      FUN_0049c260((int)param_1);
      if ((*(int *)(param_1 + 0x1c) != 7) && (*(int *)(param_1 + 0x1c) != 0x11)) {
        return param_1[1];
      }
      return *param_1;
    }
  }
  if (*(int *)(param_1 + 0x1c) == 7) {
    *param_1 = *param_1 + param_1[1];
    param_1[4] = *param_1;
  }
  else if (*(int *)(param_1 + 0x1c) == 0x11) {
    *param_1 = *param_1 + param_1[3];
    param_1[3] = param_1[3] + param_1[1];
    param_1[4] = *param_1;
  }
  else if (*(int *)(param_1 + 0x1c) == 8) {
    fVar2 = FUN_00423bd0((int)(param_1 + 8));
    fVar3 = (float)fVar2 / (float)*(int *)(param_1 + 0x18);
    param_1[4] = (byte)(int)((float)*param_1 *
                             (fVar3 - DAT_0056c8cc) * (fVar3 - DAT_0056c8cc) *
                             (DAT_0056c8d0 * fVar3 + DAT_0056c8cc) +
                             (float)param_1[1] *
                             fVar3 * fVar3 * (DAT_0056c8d8 - DAT_0056c8d0 * fVar3) +
                             (float)param_1[2] *
                             (DAT_0056c8cc - fVar3) * (DAT_0056c8cc - fVar3) * fVar3 +
                            (float)param_1[3] * (fVar3 - DAT_0056c8cc) * fVar3 * fVar3);
  }
  else {
    fVar2 = (float10)FUN_00513ce0((int)param_1);
    param_1[4] = (byte)(int)((float)(int)((uint)param_1[1] - (uint)*param_1) * (float)fVar2 +
                            (float)*param_1);
  }
  return param_1[4];
}

