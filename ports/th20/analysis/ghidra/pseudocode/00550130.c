/* Entry: 0x00550130; symbol: FUN_00550130; body bytes: 339 */

float10 __cdecl FUN_00550130(double param_1)

{
  uint uVar1;
  int iVar2;
  ushort in_FPUControlWord;
  float10 fVar3;
  undefined1 auVar4 [16];
  
  if (1 < DAT_005e5288) {
    auVar4._8_8_ = 0;
    auVar4._0_8_ = param_1;
    auVar4 = roundsd(auVar4,auVar4,9);
    return (float10)auVar4._0_8_;
  }
  if ((DAT_005e59ac != 0) && ((MXCSR & 0x7f80) == 0x1f80 && (in_FPUControlWord & 0x7f) == 0x7f)) {
    fVar3 = FUN_00550150(param_1);
    return fVar3;
  }
  uVar1 = FUN_0055dbe0();
  if ((param_1._4_4_ & 0x7ff00000) != 0x7ff00000) {
    fVar3 = FUN_00564240(param_1);
    if (((double)fVar3 != param_1) && ((uVar1 & 0x20) == 0)) {
      fVar3 = (float10)FUN_00553a00(0x10,0xb,param_1,(double)fVar3,uVar1);
      return fVar3;
    }
    FUN_0055dbe0();
    return (float10)(double)fVar3;
  }
  iVar2 = FUN_005524b0(SUB84(param_1,0),param_1._4_4_);
  if (((iVar2 != 1) && (iVar2 != 2)) && (iVar2 != 3)) {
    fVar3 = (float10)FUN_00553a00(8,0xb,param_1,param_1 + DAT_0056c468,uVar1);
    return fVar3;
  }
  FUN_0055dbe0();
  return (float10)param_1;
}

