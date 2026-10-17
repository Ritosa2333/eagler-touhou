/* Entry: 0x00502b10; symbol: FUN_00502b10; body bytes: 92 */

undefined4 __fastcall FUN_00502b10(int param_1)

{
  bool bVar1;
  void *this;
  undefined4 uVar2;
  
  if (*(int *)(param_1 + 0x9c) == 1) {
    FUN_00506050((void *)(param_1 + 0x50),DAT_005732d4);
  }
  this = (void *)FUN_00421830(param_1);
  bVar1 = FUN_004235c0(this,0x14);
  if (bVar1) {
    uVar2 = 0xffffffff;
  }
  else {
    uVar2 = 0;
  }
  return uVar2;
}

