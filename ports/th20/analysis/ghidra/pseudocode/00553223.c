/* Entry: 0x00553223; symbol: FUN_00553223; body bytes: 228 */

undefined4 * __cdecl FUN_00553223(char *param_1)

{
  uint uVar1;
  undefined4 *puVar2;
  char *pcVar3;
  int iVar4;
  uint uVar5;
  char *pcVar6;
  char cVar7;
  undefined4 *local_8;
  
  uVar5 = 0;
  cVar7 = *param_1;
  pcVar6 = param_1;
  while (cVar7 != '\0') {
    uVar1 = uVar5 + 1;
    if (cVar7 == '=') {
      uVar1 = uVar5;
    }
    do {
      uVar5 = uVar1;
      cVar7 = *pcVar6;
      pcVar6 = pcVar6 + 1;
      uVar1 = uVar5;
    } while (cVar7 != '\0');
    cVar7 = *pcVar6;
  }
  puVar2 = (undefined4 *)FUN_00557b40(uVar5 + 1,4);
  local_8 = puVar2;
  if (puVar2 == (undefined4 *)0x0) {
    FUN_00557ba0((LPVOID)0x0);
    puVar2 = (undefined4 *)0x0;
  }
  else {
    for (; *param_1 != '\0'; param_1 = param_1 + (int)pcVar6) {
      pcVar6 = param_1;
      do {
        cVar7 = *pcVar6;
        pcVar6 = pcVar6 + 1;
      } while (cVar7 != '\0');
      pcVar6 = pcVar6 + (1 - (int)(param_1 + 1));
      if (*param_1 != '=') {
        pcVar3 = (char *)FUN_00557b40((uint)pcVar6,1);
        if (pcVar3 == (char *)0x0) {
          free_environment<>(puVar2);
          FUN_00557ba0((LPVOID)0x0);
          FUN_00557ba0((LPVOID)0x0);
          return (undefined4 *)0x0;
        }
        iVar4 = FUN_00548610(pcVar3,(int)pcVar6,(int)param_1);
        if (iVar4 != 0) {
                    /* WARNING: Subroutine does not return */
          __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        *local_8 = pcVar3;
        local_8 = local_8 + 1;
        FUN_00557ba0((LPVOID)0x0);
      }
    }
    FUN_00557ba0((LPVOID)0x0);
  }
  return puVar2;
}

