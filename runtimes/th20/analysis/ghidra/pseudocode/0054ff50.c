/* Entry: 0x0054ff50; symbol: FUN_0054ff50; body bytes: 67 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

uint __thiscall FUN_0054ff50(void *this,int param_1,undefined4 param_2)

{
  uint extraout_EAX;
  uint uVar1;
  uint extraout_EAX_00;
  bool bVar2;
  ushort in_FPUControlWord;
  ushort in_FPUStatusWord;
  unkbyte10 extraout_ST0;
  float10 fVar3;
  undefined4 unaff_retaddr;
  
  bVar2 = DAT_005e59ac == 0;
  if ((!bVar2) && (bVar2 = (MXCSR & 0x7f80) == 0x1f80 && (in_FPUControlWord & 0x7f) == 0x7f, bVar2))
  {
    FUN_0055acf8((double)CONCAT44(param_2,param_1));
    return extraout_EAX;
  }
  uVar1 = __fload_withFB(this,(int)&param_1);
  if (bVar2) {
    if (((uVar1 & 0xfffff) == 0) && (param_1 == 0)) {
      if (DAT_005e5998 != 0) {
        return 1;
      }
      __startOneArgErrorHandling(&DAT_0059a680,0x12,in_FPUControlWord,unaff_retaddr,0,param_2);
      uVar1 = extraout_EAX_00;
    }
    return uVar1;
  }
  fVar3 = (float10)fcos(extraout_ST0);
  uVar1 = CONCAT22((short)(uVar1 >> 0x10),in_FPUStatusWord);
  if ((in_FPUStatusWord & 0x400) != 0) {
    do {
      fVar3 = fVar3 - (fVar3 / _DAT_0059a92a) * _DAT_0059a92a;
      uVar1 = CONCAT22((short)(uVar1 >> 0x10),in_FPUStatusWord);
    } while ((in_FPUStatusWord & 0x400) != 0);
    fcos(fVar3);
  }
  if (DAT_005e5998 != 0) {
    return uVar1;
  }
  uVar1 = __math_exit(&DAT_0059a680,0x12,unaff_retaddr,param_1,param_2);
  return uVar1;
}

