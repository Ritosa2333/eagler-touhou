/* Entry: 0x00502b70; symbol: FUN_00502b70; body bytes: 216 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_00502b70(int param_1)

{
  bool bVar1;
  void *this;
  float10 fVar2;
  
  if (*(int *)(param_1 + 0x9c) == 1) {
    this = (void *)FUN_00421830(param_1);
    bVar1 = FUN_00423560(this,0x78);
    if (bVar1) {
      fVar2 = FUN_0047a320(param_1 + 0x50);
      if (_DAT_0056e0e8 < (float)fVar2) {
        FUN_00506050((void *)(param_1 + 0x50),DAT_005732d8);
        fVar2 = FUN_0047a320(param_1 + 0x50);
        if ((float)fVar2 <= DAT_0056fbf8) {
          FUN_0047a5c0((void *)(param_1 + 0x50),DAT_0056fbf8);
        }
      }
    }
    else {
      FUN_00506050((void *)(param_1 + 0x50),DAT_0056fbf8);
    }
  }
  return 0;
}

