/* Entry: 0x0055f466; symbol: copy_environment<wchar_t>; body bytes: 189 */

/* Library Function - Single Match
    wchar_t * * __cdecl copy_environment<wchar_t>(wchar_t * * const)
   
   Library: Visual Studio 2019 Release */

wchar_t ** __cdecl copy_environment<wchar_t>(wchar_t **param_1)

{
  wchar_t *pwVar1;
  uint uVar2;
  wchar_t wVar3;
  wchar_t **ppwVar4;
  LPVOID pvVar5;
  int iVar6;
  int iVar7;
  wchar_t *pwVar8;
  
  if (param_1 == (wchar_t **)0x0) {
    ppwVar4 = (wchar_t **)0x0;
  }
  else {
    iVar7 = 0;
    pwVar8 = *param_1;
    ppwVar4 = param_1;
    while (pwVar8 != (wchar_t *)0x0) {
      ppwVar4 = ppwVar4 + 1;
      iVar7 = iVar7 + 1;
      pwVar8 = *ppwVar4;
    }
    ppwVar4 = (wchar_t **)FUN_00557b40(iVar7 + 1,4);
    if (ppwVar4 == (wchar_t **)0x0) {
LAB_0055f512:
                    /* WARNING: Subroutine does not return */
      _abort();
    }
    pwVar8 = *param_1;
    if (pwVar8 != (wchar_t *)0x0) {
      iVar7 = (int)ppwVar4 - (int)param_1;
      do {
        pwVar1 = pwVar8 + 1;
        do {
          wVar3 = *pwVar8;
          pwVar8 = pwVar8 + 1;
        } while (wVar3 != L'\0');
        uVar2 = ((int)pwVar8 - (int)pwVar1 >> 1) + 1;
        pvVar5 = FUN_00557b40(uVar2,2);
        *(LPVOID *)(iVar7 + (int)param_1) = pvVar5;
        FUN_00557ba0((LPVOID)0x0);
        if (*(int *)(iVar7 + (int)param_1) == 0) goto LAB_0055f512;
        iVar6 = FUN_00548100(*(short **)(iVar7 + (int)param_1),uVar2,(int)*param_1);
        if (iVar6 != 0) {
                    /* WARNING: Subroutine does not return */
          __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        param_1 = param_1 + 1;
        pwVar8 = *param_1;
      } while (pwVar8 != (wchar_t *)0x0);
    }
    FUN_00557ba0((LPVOID)0x0);
  }
  return ppwVar4;
}

