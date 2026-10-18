/* Entry: 0x00561ae6; symbol: _ProcessCodePage; body bytes: 225 */

/* Library Function - Single Match
    _ProcessCodePage
   
   Library: Visual Studio 2019 Release */

UINT __cdecl _ProcessCodePage(wchar_t *param_1,int param_2)

{
  wchar_t wVar1;
  uint uVar2;
  int iVar3;
  UINT UVar4;
  uint uVar5;
  wchar_t *pwVar6;
  wchar_t *pwVar7;
  bool bVar8;
  UINT local_8;
  
  if ((param_1 != (wchar_t *)0x0) && (uVar2 = 0, *param_1 != L'\0')) {
    pwVar7 = L"ACP";
    pwVar6 = param_1;
    do {
      wVar1 = *pwVar6;
      bVar8 = (ushort)wVar1 < (ushort)*pwVar7;
      if (wVar1 != *pwVar7) {
LAB_00561b31:
        uVar5 = -(uint)bVar8 | 1;
        goto LAB_00561b36;
      }
      if (wVar1 == L'\0') break;
      wVar1 = pwVar6[1];
      bVar8 = (ushort)wVar1 < (ushort)pwVar7[1];
      if (wVar1 != pwVar7[1]) goto LAB_00561b31;
      pwVar6 = pwVar6 + 2;
      pwVar7 = pwVar7 + 2;
    } while (wVar1 != L'\0');
    uVar5 = 0;
LAB_00561b36:
    if (uVar5 != 0) {
      pwVar7 = L"OCP";
      pwVar6 = param_1;
      do {
        wVar1 = *pwVar6;
        bVar8 = (ushort)wVar1 < (ushort)*pwVar7;
        if (wVar1 != *pwVar7) {
LAB_00561b65:
          uVar2 = -(uint)bVar8 | 1;
          break;
        }
        if (wVar1 == L'\0') break;
        wVar1 = pwVar6[1];
        bVar8 = (ushort)wVar1 < (ushort)pwVar7[1];
        if (wVar1 != pwVar7[1]) goto LAB_00561b65;
        pwVar6 = pwVar6 + 2;
        pwVar7 = pwVar7 + 2;
      } while (wVar1 != L'\0');
      if (uVar2 != 0) {
        uVar2 = FUN_0055108d(param_1);
        return uVar2;
      }
      iVar3 = GetLocaleInfoW(*(LCID *)(param_2 + 8),0x2000000b,(LPWSTR)&local_8,2);
      if (iVar3 == 0) {
        return 0;
      }
      return local_8;
    }
  }
  iVar3 = GetLocaleInfoW(*(LCID *)(param_2 + 8),0x20001004,(LPWSTR)&local_8,2);
  if (iVar3 == 0) {
    return 0;
  }
  if (local_8 == 0) {
    UVar4 = GetACP();
    return UVar4;
  }
  return local_8;
}

