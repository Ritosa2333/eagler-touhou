/* Entry: 0x00511af0; symbol: FUN_00511af0; body bytes: 499 */

byte __fastcall FUN_00511af0(byte *param_1)

{
  float10 fVar1;
  float fVar2;
  
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
    fVar1 = FUN_00423bd0((int)(param_1 + 8));
    fVar2 = (float)fVar1 / (float)*(int *)(param_1 + 0x18);
    param_1[4] = (byte)(int)((float)*param_1 *
                             (fVar2 - DAT_0056c8cc) * (fVar2 - DAT_0056c8cc) *
                             (DAT_0056c8d0 * fVar2 + DAT_0056c8cc) +
                             (float)param_1[1] *
                             fVar2 * fVar2 * (DAT_0056c8d8 - DAT_0056c8d0 * fVar2) +
                             (float)param_1[2] *
                             (DAT_0056c8cc - fVar2) * (DAT_0056c8cc - fVar2) * fVar2 +
                            (float)param_1[3] * (fVar2 - DAT_0056c8cc) * fVar2 * fVar2);
  }
  else {
    fVar1 = (float10)FUN_00513ce0((int)param_1);
    param_1[4] = (byte)(int)((float)(int)((uint)param_1[1] - (uint)*param_1) * (float)fVar1 +
                            (float)*param_1);
  }
  return param_1[4];
}

