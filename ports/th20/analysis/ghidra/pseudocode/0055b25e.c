/* Entry: 0x0055b25e; symbol: FUN_0055b25e; body bytes: 425 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined * __cdecl FUN_0055b25e(int param_1)

{
  ushort uVar1;
  undefined *puVar2;
  undefined *extraout_EAX;
  undefined4 in_ECX;
  bool bVar3;
  ushort in_FPUControlWord;
  ushort in_FPUStatusWord;
  unkbyte10 extraout_ST0;
  float10 fVar4;
  double in_XMM0_Qa;
  undefined4 unaff_retaddr;
  undefined4 in_stack_00000008;
  
  uVar1 = ((ushort)((ulonglong)in_XMM0_Qa >> 0x30) & 0x7fff) + 0xcfd0;
  bVar3 = uVar1 == 0x10c5;
  if (uVar1 < 0x10c6) {
    return &DAT_005a07e0 + ((int)ROUND(DAT_0059a810 * in_XMM0_Qa) + 0x1c7600U & 0x3f) * 0x20;
  }
  if ((short)uVar1 < 0x10c6) {
    uVar1 = uVar1 >> 4;
    if (uVar1 != 0xcfd) {
      return (undefined *)(uint)uVar1;
    }
    return (undefined *)(uint)uVar1;
  }
  puVar2 = (undefined *)__fload_withFB(in_ECX,(int)&param_1);
  if (bVar3) {
    if ((((uint)puVar2 & 0xfffff) == 0) && (param_1 == 0)) {
      if (DAT_005e5998 != 0) {
        return (undefined *)0x1;
      }
      __startOneArgErrorHandling
                (&DAT_0059a7c0,0x1e,in_FPUControlWord,unaff_retaddr,0,in_stack_00000008);
      puVar2 = extraout_EAX;
    }
    return puVar2;
  }
  fVar4 = (float10)fsin(extraout_ST0);
  puVar2 = (undefined *)CONCAT22((short)((uint)puVar2 >> 0x10),in_FPUStatusWord);
  if ((in_FPUStatusWord & 0x400) != 0) {
    do {
      fVar4 = fVar4 - (fVar4 / _DAT_0059a92a) * _DAT_0059a92a;
      puVar2 = (undefined *)CONCAT22((short)((uint)puVar2 >> 0x10),in_FPUStatusWord);
    } while ((in_FPUStatusWord & 0x400) != 0);
    fsin(fVar4);
  }
  if (DAT_005e5998 != 0) {
    return puVar2;
  }
  puVar2 = (undefined *)__math_exit(&DAT_0059a7c0,0x1e,unaff_retaddr,param_1,in_stack_00000008);
  return puVar2;
}

