/* Entry: 0x005611a0; symbol: FUN_005611a0; body bytes: 240 */

wchar_t * __thiscall FUN_005611a0(void *this,wchar_t *param_1,int param_2)

{
  wchar_t wVar1;
  uint uVar2;
  int iVar3;
  wchar_t *pwVar4;
  wchar_t *pwVar5;
  uint uVar6;
  bool bVar7;
  LCTYPE LVar8;
  wchar_t *local_8;
  
  pwVar5 = (wchar_t *)this;
  local_8 = (wchar_t *)this;
  if ((param_1 != (wchar_t *)0x0) && (uVar6 = 0, *param_1 != L'\0')) {
    pwVar5 = L"ACP";
    pwVar4 = param_1;
    do {
      wVar1 = *pwVar4;
      bVar7 = (ushort)wVar1 < (ushort)*pwVar5;
      if (wVar1 != *pwVar5) {
LAB_005611eb:
        uVar2 = -(uint)bVar7 | 1;
        goto LAB_005611f0;
      }
      if (wVar1 == L'\0') break;
      wVar1 = pwVar4[1];
      bVar7 = (ushort)wVar1 < (ushort)pwVar5[1];
      if (wVar1 != pwVar5[1]) goto LAB_005611eb;
      pwVar4 = pwVar4 + 2;
      pwVar5 = pwVar5 + 2;
    } while (wVar1 != L'\0');
    uVar2 = 0;
LAB_005611f0:
    if (uVar2 != 0) {
      iVar3 = FUN_0055df10((ushort *)param_1,(ushort *)L"utf8");
      if (iVar3 == 0) {
        return (wchar_t *)0xfde9;
      }
      iVar3 = FUN_0055df10((ushort *)param_1,(ushort *)L"utf-8");
      if (iVar3 == 0) {
        return (wchar_t *)0xfde9;
      }
      pwVar5 = L"OCP";
      pwVar4 = param_1;
      do {
        wVar1 = *pwVar4;
        bVar7 = (ushort)wVar1 < (ushort)*pwVar5;
        if (wVar1 != *pwVar5) {
LAB_00561241:
          uVar6 = -(uint)bVar7 | 1;
          break;
        }
        if (wVar1 == L'\0') break;
        wVar1 = pwVar4[1];
        bVar7 = (ushort)wVar1 < (ushort)pwVar5[1];
        if (wVar1 != pwVar5[1]) goto LAB_00561241;
        pwVar4 = pwVar4 + 2;
        pwVar5 = pwVar5 + 2;
      } while (wVar1 != L'\0');
      if (uVar6 != 0) {
        pwVar5 = (wchar_t *)FUN_0055108d(param_1);
        return pwVar5;
      }
      LVar8 = 0x2000000b;
      goto LAB_0056127c;
    }
  }
  LVar8 = 0x20001004;
LAB_0056127c:
  iVar3 = ___acrt_GetLocaleInfoEx_16(pwVar5,(ushort *)(param_2 + 0x250),LVar8,(LPWSTR)&local_8,2);
  if (iVar3 == 0) {
    return (wchar_t *)0x0;
  }
  if ((int)local_8 < 3) {
    return (wchar_t *)0xfde9;
  }
  return local_8;
}

