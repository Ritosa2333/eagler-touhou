/* Entry: 0x00435520; symbol: FUN_00435520; body bytes: 931 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_00435520(int param_1)

{
  float10 fVar1;
  
  if (*(float *)(param_1 + 0x44) != _DAT_0056e0e8) {
    fVar1 = FUN_004292e0((float *)&DAT_005aefe4);
    fVar1 = (float10)FUN_00438600(*(float *)(param_1 + 0x38),
                                  (float)fVar1 * *(float *)(param_1 + 0x44));
    *(float *)(param_1 + 0x38) = (float)fVar1;
    *(uint *)(param_1 + 0x49c) = *(uint *)(param_1 + 0x49c) | 2;
  }
  if (*(float *)(param_1 + 0x48) != _DAT_0056e0e8) {
    fVar1 = FUN_004292e0((float *)&DAT_005aefe4);
    fVar1 = (float10)FUN_00438600(*(float *)(param_1 + 0x3c),
                                  (float)fVar1 * *(float *)(param_1 + 0x48));
    *(float *)(param_1 + 0x3c) = (float)fVar1;
    *(uint *)(param_1 + 0x49c) = *(uint *)(param_1 + 0x49c) | 2;
  }
  if (*(float *)(param_1 + 0x4c) != _DAT_0056e0e8) {
    fVar1 = FUN_004292e0((float *)&DAT_005aefe4);
    fVar1 = (float10)FUN_00438600(*(float *)(param_1 + 0x40),
                                  (float)fVar1 * *(float *)(param_1 + 0x4c));
    *(float *)(param_1 + 0x40) = (float)fVar1;
    *(uint *)(param_1 + 0x49c) = *(uint *)(param_1 + 0x49c) | 2;
  }
  if (*(float *)(param_1 + 100) != _DAT_0056e0e8) {
    fVar1 = FUN_004292e0((float *)&DAT_005aefe4);
    *(float *)(param_1 + 0x54) =
         *(float *)(param_1 + 0x54) + (float)fVar1 * *(float *)(param_1 + 100);
    *(uint *)(param_1 + 0x49c) = *(uint *)(param_1 + 0x49c) | 4;
  }
  if (*(float *)(param_1 + 0x60) != _DAT_0056e0e8) {
    fVar1 = FUN_004292e0((float *)&DAT_005aefe4);
    *(float *)(param_1 + 0x50) =
         *(float *)(param_1 + 0x50) + (float)fVar1 * *(float *)(param_1 + 0x60);
    *(uint *)(param_1 + 0x49c) = *(uint *)(param_1 + 0x49c) | 4;
  }
  if (*(float *)(param_1 + 0x3a0) != _DAT_0056e0e8) {
    fVar1 = FUN_004292e0((float *)&DAT_005aefe4);
    *(float *)(param_1 + 0x78) =
         *(float *)(param_1 + 0x78) + (float)fVar1 * *(float *)(param_1 + 0x3a0);
    if (*(float *)(param_1 + 0x78) < DAT_0056c8d0) {
      if (*(float *)(param_1 + 0x78) <= 0.0 && *(float *)(param_1 + 0x78) != 0.0) {
        *(float *)(param_1 + 0x78) = *(float *)(param_1 + 0x78) + DAT_0056c8d0;
      }
    }
    else {
      *(float *)(param_1 + 0x78) = *(float *)(param_1 + 0x78) - DAT_0056c8d0;
    }
  }
  if (*(float *)(param_1 + 0x3a4) != _DAT_0056e0e8) {
    fVar1 = FUN_004292e0((float *)&DAT_005aefe4);
    *(float *)(param_1 + 0x7c) =
         *(float *)(param_1 + 0x7c) + (float)fVar1 * *(float *)(param_1 + 0x3a4);
    if (*(float *)(param_1 + 0x7c) < DAT_0056c8d0) {
      if (*(float *)(param_1 + 0x7c) <= 0.0 && *(float *)(param_1 + 0x7c) != 0.0) {
        *(float *)(param_1 + 0x7c) = *(float *)(param_1 + 0x7c) + DAT_0056c8d0;
      }
    }
    else {
      *(float *)(param_1 + 0x7c) = *(float *)(param_1 + 0x7c) - DAT_0056c8d0;
    }
  }
  return;
}

