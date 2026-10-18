/* Entry: 0x00546538; symbol: ___FrameUnwindFilter; body bytes: 86 */

/* Library Function - Single Match
    ___FrameUnwindFilter
   
   Library: Visual Studio 2019 Release */

undefined4 __cdecl ___FrameUnwindFilter(undefined4 *param_1)

{
  int *piVar1;
  code *pcVar2;
  int iVar3;
  undefined4 uVar4;
  
  piVar1 = (int *)*param_1;
  if ((*piVar1 == -0x1fbcbcae) || (*piVar1 == -0x1fbcb0b3)) {
    iVar3 = ___vcrt_getptd();
    if (0 < *(int *)(iVar3 + 0x18)) {
      iVar3 = ___vcrt_getptd();
      *(int *)(iVar3 + 0x18) = *(int *)(iVar3 + 0x18) + -1;
    }
  }
  else if (*piVar1 == -0x1f928c9d) {
    iVar3 = ___vcrt_getptd();
    *(int **)(iVar3 + 0x10) = piVar1;
    uVar4 = param_1[1];
    iVar3 = ___vcrt_getptd();
    *(undefined4 *)(iVar3 + 0x14) = uVar4;
    terminate();
    pcVar2 = (code *)swi(3);
    uVar4 = (*pcVar2)();
    return uVar4;
  }
  return 0;
}

