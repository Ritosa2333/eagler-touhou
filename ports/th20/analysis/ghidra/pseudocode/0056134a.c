/* Entry: 0x0056134a; symbol: FUN_0056134a; body bytes: 526 */

undefined4 __cdecl FUN_0056134a(short *param_1,undefined4 *param_2,LPWSTR param_3)

{
  int *piVar1;
  short *psVar2;
  LPWSTR pWVar3;
  short sVar4;
  bool bVar5;
  int iVar6;
  undefined3 extraout_var;
  wchar_t *_Val;
  BOOL BVar7;
  ushort *puVar8;
  int *extraout_ECX;
  int *piVar9;
  short *psVar10;
  void *this;
  void *this_00;
  void *this_01;
  
  iVar6 = FUN_005579c2();
  psVar2 = (short *)(iVar6 + 0x2a0);
  *(undefined4 *)(iVar6 + 0x58) = 0;
  *psVar2 = 0;
  piVar1 = (int *)(iVar6 + 0x50);
  *piVar1 = (int)param_1;
  *(int *)(iVar6 + 0x54) = (int)(param_1 + 0x40);
  if (param_1[0x40] != 0) {
    _TranslateName(0x59ad40,0x16,(int *)(iVar6 + 0x54));
  }
  piVar9 = piVar1;
  if (*(short *)*piVar1 == 0) {
    _GetLocaleNameFromDefault((undefined4 *)(iVar6 + 0x54),(int)piVar1);
LAB_005613f4:
    if (*(int *)(iVar6 + 0x58) == 0) {
      return 0;
    }
  }
  else {
    if (**(short **)(iVar6 + 0x54) == 0) {
      _GetLocaleNameFromLanguage(piVar1);
    }
    else {
      _GetLocaleNameFromLangCountry(piVar1);
    }
    if (*(int *)(iVar6 + 0x58) == 0) {
      bVar5 = _TranslateName(0x59aa30,0x40,piVar1);
      piVar9 = extraout_ECX;
      if (CONCAT31(extraout_var,bVar5) != 0) {
        if (**(short **)(iVar6 + 0x54) == 0) {
          piVar9 = piVar1;
          _GetLocaleNameFromLanguage(piVar1);
        }
        else {
          piVar9 = piVar1;
          _GetLocaleNameFromLangCountry(piVar1);
        }
      }
      goto LAB_005613f4;
    }
  }
  if (((param_1 == (short *)0x0) || (*param_1 != 0)) || (param_1[0x80] != 0)) {
    _Val = FUN_005611a0(piVar9,(wchar_t *)
                               (-(uint)(param_1 != (short *)0x0) & (uint)(param_1 + 0x80)),
                        (int)piVar1);
  }
  else {
    _Val = (wchar_t *)GetACP();
  }
  if (_Val == (wchar_t *)0x0) {
    return 0;
  }
  if (_Val == (wchar_t *)0xfde8) {
    return 0;
  }
  BVar7 = IsValidCodePage((uint)_Val & 0xffff);
  if (BVar7 == 0) {
    return 0;
  }
  if (param_2 != (undefined4 *)0x0) {
    *param_2 = _Val;
  }
  if (param_3 == (LPWSTR)0x0) {
    return 1;
  }
  pWVar3 = param_3 + 0x90;
  *pWVar3 = L'\0';
  psVar10 = psVar2;
  do {
    sVar4 = *psVar10;
    psVar10 = psVar10 + 1;
  } while (sVar4 != 0);
  iVar6 = FUN_0055f860(pWVar3,0x55,(int)psVar2,((int)psVar10 - (iVar6 + 0x2a2) >> 1) + 1);
  if (iVar6 != 0) goto LAB_0056154c;
  iVar6 = ___acrt_GetLocaleInfoEx_16(this,(ushort *)pWVar3,0x1001,param_3,0x40);
  if (iVar6 == 0) {
    return 0;
  }
  pWVar3 = param_3 + 0x40;
  iVar6 = ___acrt_GetLocaleInfoEx_16(this_00,(ushort *)(param_3 + 0x90),0x1002,pWVar3,0x40);
  if (iVar6 == 0) {
    return 0;
  }
  this_01 = (void *)0x5f;
  puVar8 = FUN_0054628e((ushort *)pWVar3,0x5f);
  if (puVar8 == (ushort *)0x0) {
    this_01 = (void *)0x2e;
    puVar8 = FUN_0054628e((ushort *)pWVar3,0x2e);
    if (puVar8 != (ushort *)0x0) goto LAB_005614f7;
  }
  else {
LAB_005614f7:
    iVar6 = ___acrt_GetLocaleInfoEx_16(this_01,(ushort *)(param_3 + 0x90),7,pWVar3,0x40);
    if (iVar6 == 0) {
      return 0;
    }
  }
  if (_Val == (wchar_t *)0xfde9) {
    iVar6 = FUN_0055f860(param_3 + 0x80,0x10,0x59b7c4,5);
    if (iVar6 != 0) {
LAB_0056154c:
                    /* WARNING: Subroutine does not return */
      __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
  }
  else {
    FID_conflict___itow_s((long)_Val,param_3 + 0x80,0x10,10);
  }
  return 1;
}

