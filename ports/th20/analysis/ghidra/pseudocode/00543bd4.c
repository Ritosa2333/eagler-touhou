/* Entry: 0x00543bd4; symbol: ___scrt_unhandled_exception_filter@4; body bytes: 86 */

/* Library Function - Single Match
    ___scrt_unhandled_exception_filter@4
   
   Library: Visual Studio 2019 Release */

undefined4 ___scrt_unhandled_exception_filter_4(int *param_1)

{
  int iVar1;
  code *pcVar2;
  int *piVar3;
  int *piVar4;
  undefined4 uVar5;
  
  piVar4 = (int *)*param_1;
  if (((*piVar4 == -0x1f928c9d) && (piVar4[4] == 3)) &&
     ((iVar1 = piVar4[5], iVar1 == 0x19930520 ||
      (((iVar1 == 0x19930521 || (iVar1 == 0x19930522)) || (iVar1 == 0x1994000)))))) {
    piVar3 = (int *)FUN_0054658e();
    *piVar3 = (int)piVar4;
    iVar1 = param_1[1];
    piVar4 = (int *)FUN_00546597();
    *piVar4 = iVar1;
    terminate();
    pcVar2 = (code *)swi(3);
    uVar5 = (*pcVar2)();
    return uVar5;
  }
  return 0;
}

