/* Entry: 0x004ab4c0; symbol: FUN_004ab4c0; body bytes: 168 */

undefined4 __fastcall FUN_004ab4c0(int param_1)

{
  void *this;
  undefined4 uVar1;
  int iVar2;
  float10 fVar3;
  
  if ((*(uint *)(param_1 + 0x2cc) >> 0x1a & 1) == 0) {
    *(uint *)(param_1 + 0x2cc) = *(uint *)(param_1 + 0x2cc) | 0x4000000;
    iVar2 = FUN_004a7710(param_1);
    if (iVar2 == 0) {
      this = *(void **)(param_1 + 8);
      fVar3 = (float10)FUN_00456240(param_1 + 0xa8);
      iVar2 = FUN_0053e2b0(this,(float)fVar3);
      if (iVar2 == 0) {
        if ((*(int *)(param_1 + 0x2e0) != 0) &&
           (iVar2 = (**(code **)(param_1 + 0x2e0))(), iVar2 != 0)) {
          return 0xffffffff;
        }
        uVar1 = 0;
      }
      else {
        uVar1 = 0xffffffff;
      }
    }
    else {
      uVar1 = 0xffffffff;
    }
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}

