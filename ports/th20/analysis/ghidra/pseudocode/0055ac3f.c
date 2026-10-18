/* Entry: 0x0055ac3f; symbol: FUN_0055ac3f; body bytes: 147 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

uint __thiscall FUN_0055ac3f(void *this,int param_1,undefined4 param_2)

{
  uint uVar1;
  uint extraout_EAX;
  undefined1 in_ZF;
  ushort in_FPUControlWord;
  ushort in_FPUStatusWord;
  unkbyte10 extraout_ST0;
  float10 fVar2;
  undefined4 unaff_retaddr;
  
  uVar1 = __fload_withFB(this,(int)&param_1);
  if ((bool)in_ZF) {
    if (((uVar1 & 0xfffff) == 0) && (param_1 == 0)) {
      if (DAT_005e5998 != 0) {
        return 1;
      }
      __startOneArgErrorHandling(&DAT_0059a680,0x12,in_FPUControlWord,unaff_retaddr,0,param_2);
      uVar1 = extraout_EAX;
    }
    return uVar1;
  }
  fVar2 = (float10)fcos(extraout_ST0);
  uVar1 = CONCAT22((short)(uVar1 >> 0x10),in_FPUStatusWord);
  if ((in_FPUStatusWord & 0x400) != 0) {
    do {
      fVar2 = fVar2 - (fVar2 / _DAT_0059a92a) * _DAT_0059a92a;
      uVar1 = CONCAT22((short)(uVar1 >> 0x10),in_FPUStatusWord);
    } while ((in_FPUStatusWord & 0x400) != 0);
    fcos(fVar2);
  }
  if (DAT_005e5998 != 0) {
    return uVar1;
  }
  uVar1 = __math_exit(&DAT_0059a680,0x12,unaff_retaddr,param_1,param_2);
  return uVar1;
}

