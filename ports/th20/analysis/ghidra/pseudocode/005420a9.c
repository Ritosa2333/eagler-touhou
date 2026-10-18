/* Entry: 0x005420a9; symbol: FUN_005420a9; body bytes: 236 */

uint __cdecl FUN_005420a9(uint param_1,UINT *param_2)

{
  ushort uVar1;
  wchar_t **ppwVar2;
  UINT UVar3;
  int iVar4;
  ushort *puVar5;
  uint uVar6;
  wchar_t *pwVar7;
  bool bVar8;
  byte local_c;
  undefined1 local_b;
  char local_8;
  char local_7;
  undefined1 local_6;
  
  if (param_2 == (UINT *)0x0) {
    ppwVar2 = ____lc_locale_name_func();
    pwVar7 = ppwVar2[2];
    UVar3 = ____lc_codepage_func();
  }
  else {
    pwVar7 = (wchar_t *)param_2[3];
    UVar3 = *param_2;
  }
  if (pwVar7 == (LPCWSTR)0x0) {
    uVar6 = param_1 + 0x20;
    if (0x19 < param_1 - 0x41) {
      uVar6 = param_1;
    }
  }
  else {
    if (param_1 < 0x100) {
      if (param_2 == (UINT *)0x0) {
        iVar4 = _isupper(param_1);
        bVar8 = iVar4 == 0;
      }
      else {
        bVar8 = (*(byte *)(param_2[1] + param_1 * 2) & 1) == 0;
      }
      if (bVar8) {
        return param_1;
      }
    }
    if (param_2 == (UINT *)0x0) {
      puVar5 = ___pctype_func();
      uVar1 = puVar5[(int)param_1 >> 8 & 0xff] & 0x8000;
    }
    else {
      uVar1 = *(ushort *)(param_2[1] + ((int)param_1 >> 8 & 0xffU) * 2) >> 0xf;
    }
    if (uVar1 == 0) {
      local_7 = '\0';
      iVar4 = 1;
      local_8 = (char)param_1;
    }
    else {
      local_6 = 0;
      iVar4 = 2;
      local_8 = (char)(param_1 >> 8);
      local_7 = (char)param_1;
    }
    iVar4 = FUN_0054256f(pwVar7,0x100,&local_8,iVar4,(LPWSTR)&local_c,3,UVar3,1);
    uVar6 = param_1;
    if ((iVar4 != 0) && (uVar6 = (uint)local_c, iVar4 != 1)) {
      uVar6 = (uint)CONCAT11(local_c,local_b);
    }
  }
  return uVar6;
}

