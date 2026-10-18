/* Entry: 0x00502a20; symbol: FUN_00502a20; body bytes: 174 */

undefined4 __fastcall FUN_00502a20(int param_1)

{
  float *pfVar1;
  float10 fVar2;
  float fVar3;
  
  if ((*(int *)(param_1 + 0x9c) == 1) &&
     ((fVar2 = FUN_004292e0((float *)(param_1 + 0x50)), (float)fVar2 < DAT_0056fa6c ||
      (fVar2 = FUN_004292e0((float *)(param_1 + 0x50)), DAT_0056e050 < (float)fVar2)))) {
    fVar3 = (DAT_0056e0f8 / DAT_0056c8d0) * DAT_0056c8d0;
    pfVar1 = (float *)FUN_00421830(param_1 + 0x50);
    fVar2 = FUN_004292e0(pfVar1);
    FUN_0047a560((void *)(param_1 + 0x50),fVar3 - (float)fVar2);
  }
  return 0;
}

