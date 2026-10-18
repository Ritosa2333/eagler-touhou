/* Entry: 0x0055b904; symbol: FUN_0055b904; body bytes: 157 */

uint __cdecl FUN_0055b904(int param_1,uint param_2)

{
  uint uVar1;
  uint extraout_EAX;
  bool in_ZF;
  ushort in_FPUControlWord;
  undefined4 unaff_retaddr;
  
  if (in_ZF) {
    if (((param_2 & 0xfffff) != 0) || (param_1 != 0)) {
      uVar1 = FUN_0055b78c();
      goto LAB_0055b982;
    }
    uVar1 = param_2 & 0x80000000;
    if (uVar1 == 0) goto LAB_0055b925;
  }
  else {
    uVar1 = param_2;
    if (in_FPUControlWord != 0x27f) {
      uVar1 = FUN_0055b775();
    }
    if (((uVar1 & 0x80000000) == 0) ||
       ((((uVar1 & 0x7ff00000) == 0 && ((uVar1 & 0xfffff) == 0)) && (param_1 == 0)))) {
LAB_0055b925:
      if (DAT_005e5998 != 0) {
        return uVar1;
      }
      uVar1 = __math_exit(&DAT_0059a970,5,unaff_retaddr,param_1,param_2);
      return uVar1;
    }
  }
  uVar1 = 1;
LAB_0055b982:
  if (DAT_005e5998 != 0) {
    return uVar1;
  }
  __startOneArgErrorHandling(&DAT_0059a970,5,in_FPUControlWord,unaff_retaddr,param_1,param_2);
  return extraout_EAX;
}

