/* Entry: 0x00502c50; symbol: FUN_00502c50; body bytes: 341 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_00502c50(int param_1)

{
  int iVar1;
  float *pfVar2;
  float10 fVar3;
  float fVar4;
  
  iVar1 = FUN_00472040((void *)(param_1 + 0x1c),0x1e);
  if (iVar1 == 0) {
    fVar3 = FUN_004298e0(0x5ba4a8);
    fVar4 = DAT_0056e0f0 / _DAT_00570db0;
    pfVar2 = (float *)FUN_00414580((void *)(param_1 + 0xb8),0);
    *pfVar2 = (float)fVar3 * fVar4;
    fVar3 = FUN_004298e0(0x5ba4a8);
    fVar4 = (float)fVar3 * DAT_0056fbf8;
    pfVar2 = (float *)FUN_00414580((void *)(param_1 + 0xb8),1);
    *pfVar2 = fVar4;
  }
  pfVar2 = (float *)FUN_00414580((void *)(param_1 + 0xb8),0);
  FUN_0047a240((void *)(param_1 + 0x50),*pfVar2);
  fVar3 = FUN_0047a320(param_1 + 0x50);
  if (((float)fVar3 < DAT_0056c8d0) &&
     (pfVar2 = (float *)FUN_00414580((void *)(param_1 + 0xb8),1), *pfVar2 < _DAT_0056e0e8)) {
    return 0;
  }
  pfVar2 = (float *)FUN_00414580((void *)(param_1 + 0xb8),1);
  FUN_00506050((void *)(param_1 + 0x50),*pfVar2);
  return 0;
}

