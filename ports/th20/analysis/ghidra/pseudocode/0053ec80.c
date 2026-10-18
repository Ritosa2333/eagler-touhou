/* Entry: 0x0053ec80; symbol: FUN_0053ec80; body bytes: 190 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

float10 __thiscall FUN_0053ec80(int param_1,byte param_2,float param_3)

{
  code *pcVar1;
  int iVar2;
  float *pfVar3;
  float10 fVar4;
  
  iVar2 = FUN_005403d0(param_1);
  if (((uint)*(ushort *)(iVar2 + 8) & 1 << (param_2 & 0x1f)) == 0) {
    fVar4 = (float10)param_3;
  }
  else if (param_3 < _DAT_0056e0e8) {
    if ((DAT_0056e0f4 < param_3) || (param_3 < DAT_005732dc)) {
      pcVar1 = *(code **)(**(int **)(param_1 + 0x28) + 0x10);
      fVar4 = (float10)(*pcVar1)((int)param_3,pcVar1,*(undefined4 *)(param_1 + 0x28));
    }
    else {
      FUN_0053f0b0((void *)(param_1 + 0xc),4,&param_3,'f');
      fVar4 = (float10)param_3;
    }
  }
  else {
    pfVar3 = (float *)FUN_0053e6a0((void *)(param_1 + 0xc),(int)param_3);
    fVar4 = (float10)*pfVar3;
  }
  return fVar4;
}

