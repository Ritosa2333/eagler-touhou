/* Entry: 0x0056675f; symbol: FUN_0056675f; body bytes: 193 */

uint __thiscall FUN_0056675f(void *this,int param_1,uint param_2)

{
  uint uVar1;
  uint uVar2;
  uint extraout_EAX;
  undefined1 in_ZF;
  ushort in_FPUControlWord;
  undefined4 unaff_retaddr;
  
  uVar2 = __fload_withFB(this,(int)&param_1);
  if ((bool)in_ZF) {
    if (((uVar2 & 0xfffff) != 0) || (param_1 != 0)) {
      uVar2 = FUN_0055b78c();
      goto LAB_00566801;
    }
    uVar2 = uVar2 & 0x80000000;
    uVar1 = uVar2;
  }
  else {
    uVar1 = param_2;
    if ((param_2 & 0x7ff00000) == 0) {
      if (((param_2 & 0xfffff) == 0) && (param_1 == 0)) {
        uVar2 = 2;
        goto LAB_00566801;
      }
      uVar2 = param_2 & 0x80000000;
    }
    else {
      uVar2 = param_2 & 0x80000000;
    }
  }
  if (uVar2 == 0) {
    if (DAT_005e5998 != 0) {
      return uVar1;
    }
    uVar2 = __math_exit("log10",0x1b,unaff_retaddr,param_1,param_2);
    return uVar2;
  }
  uVar2 = 1;
LAB_00566801:
  if (DAT_005e5998 != 0) {
    return uVar2;
  }
  __startOneArgErrorHandling("log10",0x1b,in_FPUControlWord,unaff_retaddr,param_1,param_2);
  return extraout_EAX;
}

