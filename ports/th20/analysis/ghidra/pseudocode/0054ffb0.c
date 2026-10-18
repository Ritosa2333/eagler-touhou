/* Entry: 0x0054ffb0; symbol: FUN_0054ffb0; body bytes: 214 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined * __thiscall FUN_0054ffb0(void *this,int param_1,undefined4 param_2)

{
  undefined *puVar1;
  undefined *extraout_EAX;
  bool bVar2;
  ushort in_FPUControlWord;
  ushort in_FPUStatusWord;
  unkbyte10 extraout_ST0;
  float10 fVar3;
  undefined4 unaff_retaddr;
  
  bVar2 = DAT_005e59ac == 0;
  if ((!bVar2) && (bVar2 = (MXCSR & 0x7f80) == 0x1f80 && (in_FPUControlWord & 0x7f) == 0x7f, bVar2))
  {
    puVar1 = FUN_0055b258(param_1,param_2);
    return puVar1;
  }
  puVar1 = (undefined *)__fload_withFB(this,(int)&param_1);
  if (bVar2) {
    if ((((uint)puVar1 & 0xfffff) == 0) && (param_1 == 0)) {
      if (DAT_005e5998 != 0) {
        return (undefined *)0x1;
      }
      __startOneArgErrorHandling(&DAT_0059a7c0,0x1e,in_FPUControlWord,unaff_retaddr,0,param_2);
      puVar1 = extraout_EAX;
    }
    return puVar1;
  }
  fVar3 = (float10)fsin(extraout_ST0);
  puVar1 = (undefined *)CONCAT22((short)((uint)puVar1 >> 0x10),in_FPUStatusWord);
  if ((in_FPUStatusWord & 0x400) != 0) {
    do {
      fVar3 = fVar3 - (fVar3 / _DAT_0059a92a) * _DAT_0059a92a;
      puVar1 = (undefined *)CONCAT22((short)((uint)puVar1 >> 0x10),in_FPUStatusWord);
    } while ((in_FPUStatusWord & 0x400) != 0);
    fsin(fVar3);
  }
  if (DAT_005e5998 != 0) {
    return puVar1;
  }
  puVar1 = (undefined *)__math_exit(&DAT_0059a7c0,0x1e,unaff_retaddr,param_1,param_2);
  return puVar1;
}

