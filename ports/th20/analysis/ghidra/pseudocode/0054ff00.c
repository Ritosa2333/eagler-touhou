/* Entry: 0x0054ff00; symbol: FUN_0054ff00; body bytes: 201 */

void __thiscall FUN_0054ff00(void *this,int param_1,undefined4 param_2)

{
  uint uVar1;
  bool bVar2;
  ushort in_FPUControlWord;
  unkbyte10 extraout_ST0;
  undefined4 unaff_retaddr;
  
  bVar2 = DAT_005e59ac == 0;
  if ((!bVar2) && (bVar2 = (MXCSR & 0x7f80) == 0x1f80 && (in_FPUControlWord & 0x7f) == 0x7f, bVar2))
  {
    FUN_0055a938((double)CONCAT44(param_2,param_1));
    return;
  }
  uVar1 = __fload_withFB(this,(int)&param_1);
  if (bVar2) {
    if (((uVar1 & 0xfffff) != 0) || (param_1 != 0)) {
      FUN_0055b78c();
      if (DAT_005e5998 != 0) {
        return;
      }
      __startOneArgErrorHandling(&DAT_0059a520,0xf,in_FPUControlWord,unaff_retaddr,param_1,param_2);
      return;
    }
  }
  else {
    fpatan(extraout_ST0,(float10)1);
  }
  if (DAT_005e5998 != 0) {
    return;
  }
  __math_exit(&DAT_0059a520,0xf,unaff_retaddr,param_1,param_2);
  return;
}

