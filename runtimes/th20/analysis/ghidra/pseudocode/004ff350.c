/* Entry: 0x004ff350; symbol: FUN_004ff350; body bytes: 157 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

float10 __thiscall FUN_004ff350(int param_1,float *param_2)

{
  float fVar1;
  float fVar2;
  float *pfVar3;
  int iVar4;
  float10 fVar5;
  float fVar6;
  
  pfVar3 = (float *)FUN_00460850(param_1);
  fVar1 = *pfVar3;
  fVar2 = *param_2;
  iVar4 = FUN_00460850(param_1);
  fVar6 = *(float *)(iVar4 + 4) - param_2[1];
  if ((fVar6 == _DAT_0056e0e8) && (fVar1 - fVar2 == _DAT_0056e0e8)) {
    fVar5 = (float10)(DAT_0056e0f0 / DAT_0056c8d0);
  }
  else {
    fVar5 = FUN_00459280(fVar6,fVar1 - fVar2);
  }
  return fVar5;
}

