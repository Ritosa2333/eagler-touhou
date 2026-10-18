/* Entry: 0x00553308; symbol: FUN_00553308; body bytes: 290 */

undefined4 * __cdecl FUN_00553308(short *param_1)

{
  uint uVar1;
  short sVar2;
  short sVar3;
  int iVar4;
  undefined4 *puVar5;
  short *psVar6;
  int iVar7;
  short *psVar8;
  undefined4 *local_c;
  
  sVar2 = *param_1;
  iVar7 = 0;
  psVar6 = param_1;
  while (sVar2 != 0) {
    iVar4 = iVar7 + 1;
    if (sVar2 == 0x3d) {
      iVar4 = iVar7;
    }
    psVar8 = psVar6;
    do {
      sVar2 = *psVar8;
      psVar8 = psVar8 + 1;
    } while (sVar2 != 0);
    psVar6 = psVar6 + ((int)psVar8 - (int)(psVar6 + 1) >> 1) + 1;
    iVar7 = iVar4;
    sVar2 = *psVar6;
  }
  puVar5 = (undefined4 *)FUN_00557b40(iVar7 + 1,4);
  if (puVar5 == (undefined4 *)0x0) {
    FUN_00557ba0((LPVOID)0x0);
    puVar5 = (undefined4 *)0x0;
  }
  else {
    sVar2 = *param_1;
    local_c = puVar5;
    while (sVar2 != 0) {
      psVar6 = param_1;
      do {
        sVar3 = *psVar6;
        psVar6 = psVar6 + 1;
      } while (sVar3 != 0);
      uVar1 = ((int)psVar6 - (int)(param_1 + 1) >> 1) + 1;
      if (sVar2 != 0x3d) {
        psVar6 = (short *)FUN_00557b40(uVar1,2);
        if (psVar6 == (short *)0x0) {
          free_environment<>(puVar5);
          FUN_00557ba0((LPVOID)0x0);
          FUN_00557ba0((LPVOID)0x0);
          return (undefined4 *)0x0;
        }
        iVar7 = FUN_00548100(psVar6,uVar1,(int)param_1);
        if (iVar7 != 0) {
                    /* WARNING: Subroutine does not return */
          __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        *local_c = psVar6;
        local_c = local_c + 1;
        FUN_00557ba0((LPVOID)0x0);
      }
      param_1 = param_1 + uVar1;
      sVar2 = *param_1;
    }
    FUN_00557ba0((LPVOID)0x0);
  }
  return puVar5;
}

