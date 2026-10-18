/* Entry: 0x0054feb0; symbol: FUN_0054feb0; body bytes: 250 */

uint __thiscall FUN_0054feb0(void *this,int param_1,uint param_2)

{
  uint extraout_EAX;
  uint uVar1;
  uint extraout_EAX_00;
  bool bVar2;
  ushort in_FPUControlWord;
  float10 extraout_ST0;
  float10 extraout_ST0_00;
  float10 fVar3;
  undefined4 unaff_retaddr;
  
  bVar2 = DAT_005e59ac == 0;
  if ((!bVar2) && (bVar2 = (MXCSR & 0x7f80) == 0x1f80 && (in_FPUControlWord & 0x7f) == 0x7f, bVar2))
  {
    FUN_0055a2f8((double)CONCAT44(param_2,param_1));
    return extraout_EAX;
  }
  uVar1 = __fload_withFB(this,(int)&param_1);
  if (bVar2) {
    if (((uVar1 & 0xfffff) == 0) && (param_1 == 0)) {
LAB_0055a2aa:
      uVar1 = 1;
    }
    else {
      uVar1 = FUN_0055b78c();
    }
    if (DAT_005e5998 == 0) {
      __startOneArgErrorHandling(&DAT_0059a3f0,0xd,in_FPUControlWord,unaff_retaddr,param_1,param_2);
      return extraout_EAX_00;
    }
  }
  else {
    fVar3 = extraout_ST0;
    if (in_FPUControlWord != 0x27f) {
      uVar1 = FUN_0055b775();
      fVar3 = extraout_ST0_00;
    }
    if (uVar1 < 0x3ff00000) {
      fpatan(SQRT(((float10)1 - fVar3) * ((float10)1 + fVar3)),fVar3);
    }
    else {
      if ((0x3ff00000 < uVar1) || ((param_2 & 0xfffff) != 0 || param_1 != 0)) goto LAB_0055a2aa;
      uVar1 = 0;
    }
    if (DAT_005e5998 == 0) {
      uVar1 = __math_exit(&DAT_0059a3f0,0xd,unaff_retaddr,param_1,param_2);
      return uVar1;
    }
  }
  return uVar1;
}

