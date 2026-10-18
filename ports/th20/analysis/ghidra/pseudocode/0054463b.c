/* Entry: 0x0054463b; symbol: __FindAndUnlinkFrame; body bytes: 71 */

/* Library Function - Single Match
    __FindAndUnlinkFrame
   
   Library: Visual Studio 2019 Release */

void __cdecl __FindAndUnlinkFrame(int param_1)

{
  undefined4 uVar1;
  int iVar2;
  int *piVar3;
  
  iVar2 = ___vcrt_getptd();
  if (param_1 == *(int *)(iVar2 + 0x24)) {
    uVar1 = *(undefined4 *)(param_1 + 4);
    iVar2 = ___vcrt_getptd();
    *(undefined4 *)(iVar2 + 0x24) = uVar1;
  }
  else {
    iVar2 = ___vcrt_getptd();
    iVar2 = *(int *)(iVar2 + 0x24);
    do {
      piVar3 = (int *)(iVar2 + 4);
      iVar2 = *piVar3;
      if (iVar2 == 0) {
                    /* WARNING: Subroutine does not return */
        _abort();
      }
    } while (param_1 != iVar2);
    *piVar3 = *(int *)(param_1 + 4);
  }
  return;
}

