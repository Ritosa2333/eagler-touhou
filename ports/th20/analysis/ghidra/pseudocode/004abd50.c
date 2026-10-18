/* Entry: 0x004abd50; symbol: FUN_004abd50; body bytes: 437 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_004abd50(int param_1)

{
  float10 fVar1;
  double dVar2;
  
  fVar1 = (float10)FUN_0041cb10();
  dVar2 = (double)fVar1;
  if (dVar2 < *(double *)(param_1 + 0x18)) {
    *(double *)(param_1 + 0x18) = dVar2;
  }
  if (DAT_0056c468 <= dVar2 - *(double *)(param_1 + 0x18)) {
    dVar2 = dVar2 - *(double *)(param_1 + 0x18);
    *(double *)(param_1 + 0x18) = *(double *)(param_1 + 0x18) + dVar2;
    *(float *)(param_1 + 0x38) =
         (float)(((double)*(int *)(param_1 + 0x24) +
                 *(double *)(&DAT_0056cdd0 + (*(int *)(param_1 + 0x24) >> 0x1f) * -8)) / dVar2);
    if (*(float *)(param_1 + 0x38) <= _DAT_00570618) {
      *(undefined4 *)(param_1 + 0x20) = 0;
    }
    else {
      *(int *)(param_1 + 0x20) = *(int *)(param_1 + 0x20) + 1;
    }
    if (((DAT_005ba828 != 0) && ((*(uint *)(DAT_005ba828 + 0xe8) >> 4 & 1) == 0)) &&
       ((*(uint *)(DAT_005ba828 + 0xe8) >> 2 & 1) == 0)) {
      *(double *)(param_1 + 0x30) = *(double *)(param_1 + 0x30) + _DAT_0056c470;
      if (*(float *)(param_1 + 0x38) <= _DAT_00570614) {
        *(double *)(param_1 + 0x28) =
             (double)*(float *)(param_1 + 0x38) + *(double *)(param_1 + 0x28);
      }
      else {
        *(double *)(param_1 + 0x28) = *(double *)(param_1 + 0x28) + _DAT_0056c470;
      }
    }
    if (DAT_005ba828 != 0) {
      *(uint *)(DAT_005ba828 + 0xe8) = *(uint *)(DAT_005ba828 + 0xe8) & 0xfffffeff;
    }
    *(undefined4 *)(param_1 + 0x24) = 0;
  }
  *(uint *)(param_1 + 0x24) = DAT_005c4f84 + 1 + *(int *)(param_1 + 0x24);
  FUN_004ac0e0();
  return 1;
}

