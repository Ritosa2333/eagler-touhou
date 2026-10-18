/* Entry: 0x00503240; symbol: FUN_00503240; body bytes: 142 */

undefined4 __fastcall FUN_00503240(int param_1)

{
  float *pfVar1;
  float10 fVar2;
  float fVar3;
  
  if (*(int *)(param_1 + 0x9c) == 1) {
    FUN_00506050((void *)(param_1 + 0x50),DAT_0056ef28);
    fVar3 = DAT_0056e0f8 / DAT_0056c8d0;
    pfVar1 = (float *)FUN_00421830(param_1 + 0x50);
    fVar2 = FUN_004292e0(pfVar1);
    FUN_0047a240((void *)(param_1 + 0x50),(fVar3 - (float)fVar2) * DAT_0056fe5c);
  }
  return 0;
}

