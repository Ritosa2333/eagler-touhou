/* Entry: 0x004bce10; symbol: FUN_004bce10; body bytes: 223 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004bce10(int param_1)

{
  int iVar1;
  void *pvVar2;
  float10 fVar3;
  ulonglong uVar4;
  
  iVar1 = FUN_00488830(DAT_005ba828);
  if (iVar1 == 0) {
    iVar1 = FUN_004ac0c0(0x5c4d40);
    if (iVar1 != -1) {
      iVar1 = FUN_004ac0c0(0x5c4d40);
      if (iVar1 != 3) {
        fVar3 = (float10)FUN_0041cb10();
        if (_DAT_0056cd50 <= (double)fVar3 - *(double *)(param_1 + 0x2b0)) {
          pvVar2 = (void *)FUN_004640e0((int)DAT_005c6108);
          uVar4 = FUN_00542d20();
          FUN_004bcf30(pvVar2,(uint)uVar4,(int)(uVar4 >> 0x20));
          pvVar2 = (void *)FUN_0050fc50(DAT_005c6108);
          uVar4 = FUN_00542d20();
          FUN_004bcef0(pvVar2,(uint)uVar4,(int)(uVar4 >> 0x20));
        }
        fVar3 = (float10)FUN_0041cb10();
        *(double *)(param_1 + 0x2b0) = (double)fVar3;
      }
    }
  }
  return;
}

