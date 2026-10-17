/* Entry: 0x00564257; symbol: common_getenv_nolock<wchar_t>; body bytes: 132 */

/* Library Function - Single Match
    wchar_t * __cdecl common_getenv_nolock<wchar_t>(wchar_t const * const)
   
   Library: Visual Studio 2019 Release */

wchar_t * __cdecl common_getenv_nolock<wchar_t>(wchar_t *param_1)

{
  wchar_t wVar1;
  WCHAR WVar2;
  PCNZWCH pWVar3;
  int *piVar4;
  int iVar5;
  PCNZWCH pWVar6;
  wchar_t *pwVar7;
  uint uVar8;
  
  piVar4 = (int *)thunk_FUN_00553148();
  if ((piVar4 != (int *)0x0) && (param_1 != (wchar_t *)0x0)) {
    pwVar7 = param_1;
    do {
      wVar1 = *pwVar7;
      pwVar7 = pwVar7 + 1;
    } while (wVar1 != L'\0');
    uVar8 = (int)pwVar7 - (int)(param_1 + 1) >> 1;
    for (; pWVar3 = (PCNZWCH)*piVar4, pWVar3 != (PCNZWCH)0x0; piVar4 = piVar4 + 1) {
      pWVar6 = pWVar3;
      do {
        WVar2 = *pWVar6;
        pWVar6 = pWVar6 + 1;
      } while (WVar2 != L'\0');
      if (((uVar8 < (uint)((int)pWVar6 - (int)(pWVar3 + 1) >> 1)) && (pWVar3[uVar8] == L'=')) &&
         (iVar5 = FUN_005659e0(pWVar3,param_1,uVar8), iVar5 == 0)) {
        return (wchar_t *)(*piVar4 + uVar8 * 2 + 2);
      }
    }
  }
  return (wchar_t *)0x0;
}

