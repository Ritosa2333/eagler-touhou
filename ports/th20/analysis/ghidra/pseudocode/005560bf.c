/* Entry: 0x005560bf; symbol: FUN_005560bf; body bytes: 981 */

void __cdecl
FUN_005560bf(wchar_t *param_1,short *param_2,int param_3,wchar_t *param_4,uint param_5,UINT *param_6
            )

{
  wchar_t wVar1;
  UINT *pUVar2;
  char cVar3;
  int iVar4;
  uint uVar5;
  wchar_t *pwVar6;
  wchar_t *pwVar7;
  wchar_t *extraout_ECX;
  wchar_t *extraout_ECX_00;
  wchar_t *pwVar8;
  uint uVar9;
  wchar_t *pwVar10;
  uint uVar11;
  bool bVar12;
  _expandlocale_locale_name_cache local_1fc [16];
  short *local_1ec;
  UINT *local_1e8;
  undefined4 local_1e4;
  wchar_t *local_1e0;
  UINT *local_1dc;
  wchar_t *local_1d8;
  short local_1d4 [128];
  ushort local_d4;
  ushort local_d2;
  ushort local_d0;
  short local_ce;
  short local_cc;
  short local_ca;
  wchar_t local_b4;
  undefined1 local_b2 [170];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_1e8 = param_6;
  local_1ec = param_2;
  if (param_1 == (wchar_t *)0x0) goto LAB_00556479;
  local_1e4 = 0;
  if ((*param_1 == L'C') && (param_1[1] == L'\0')) {
    iVar4 = FUN_00548100(param_2,param_3,0x59979c);
    if (iVar4 == 0) {
      *local_1e8 = 0;
      goto LAB_00556479;
    }
    goto LAB_00556488;
  }
  iVar4 = FUN_005579c2();
  local_1dc = (UINT *)(iVar4 + 0x68);
  pwVar10 = (wchar_t *)(iVar4 + 0x172);
  local_1e0 = (wchar_t *)(iVar4 + 0x6c);
  local_1d8 = pwVar10;
  _expandlocale_locale_name_cache::_expandlocale_locale_name_cache
            (local_1fc,param_4,param_5,(__crt_qualified_locale_data *)(iVar4 + 0x50));
  pwVar8 = param_1 + 1;
  pwVar6 = param_1;
  do {
    wVar1 = *pwVar6;
    pwVar6 = pwVar6 + 1;
  } while (wVar1 != L'\0');
  uVar11 = (int)pwVar6 - (int)pwVar8 >> 1;
  pwVar6 = pwVar10;
  pwVar7 = param_1;
  if (uVar11 < 0x83) {
    do {
      wVar1 = *pwVar6;
      bVar12 = (ushort)wVar1 < (ushort)*pwVar7;
      if (wVar1 != *pwVar7) {
LAB_005561af:
        uVar5 = -(uint)bVar12 | 1;
        goto LAB_005561b4;
      }
      if (wVar1 == L'\0') break;
      wVar1 = pwVar6[1];
      bVar12 = (ushort)wVar1 < (ushort)pwVar7[1];
      if (wVar1 != pwVar7[1]) goto LAB_005561af;
      pwVar6 = pwVar6 + 2;
      pwVar7 = pwVar7 + 2;
    } while (wVar1 != L'\0');
    uVar5 = 0;
LAB_005561b4:
    pwVar6 = local_1e0;
    pwVar8 = param_1;
    if (uVar5 != 0) {
      do {
        wVar1 = *pwVar6;
        bVar12 = (ushort)wVar1 < (ushort)*pwVar8;
        if (wVar1 != *pwVar8) {
LAB_005561ea:
          uVar5 = -(uint)bVar12 | 1;
          goto LAB_005561ef;
        }
        if (wVar1 == L'\0') break;
        wVar1 = pwVar6[1];
        bVar12 = (ushort)wVar1 < (ushort)pwVar8[1];
        if (wVar1 != pwVar8[1]) goto LAB_005561ea;
        pwVar8 = pwVar8 + 2;
        pwVar6 = pwVar6 + 2;
      } while (wVar1 != L'\0');
      uVar5 = 0;
LAB_005561ef:
      if (uVar5 != 0) goto LAB_005561f7;
    }
LAB_00556440:
    *local_1e8 = *local_1dc;
    iVar4 = FUN_00548100(local_1ec,param_3,(int)pwVar10);
    if (iVar4 != 0) {
LAB_00556488:
                    /* WARNING: Subroutine does not return */
      __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
  }
  else {
LAB_005561f7:
    bVar12 = FUN_00558390(pwVar8);
    pwVar8 = param_1;
    iVar4 = FUN_00555ef1(local_1d4,(ushort *)param_1);
    pUVar2 = local_1dc;
    if (iVar4 == 0) {
      if (bVar12) {
        iVar4 = FUN_0056134a(local_1d4,local_1dc,local_1d4);
        pwVar8 = extraout_ECX_00;
      }
      else {
        iVar4 = FUN_00561cc8((int)local_1d4,local_1dc,local_1d4);
        pwVar8 = extraout_ECX;
      }
      pwVar10 = local_1d8;
      if (iVar4 == 0) goto LAB_0055628f;
      ___lc_lctowcs(local_1d8,0x83,(int)local_1d4);
      pwVar8 = &local_b4;
      do {
        wVar1 = *pwVar8;
        pwVar8 = pwVar8 + 1;
      } while (wVar1 != (wchar_t)local_1e4);
      _expandlocale_locale_name_cache::commit_locale_name_cache_already_updated
                (local_1fc,&local_b4,((int)pwVar8 - (int)local_b2 >> 1) + 1);
LAB_0055640b:
      if ((*param_1 == L'\0') || (0x82 < uVar11)) {
        *local_1e0 = L'\0';
      }
      else {
        iVar4 = FUN_0055f860(local_1e0,0x83,(int)param_1,uVar11 + 1);
        if (iVar4 != 0) goto LAB_00556488;
      }
      goto LAB_00556440;
    }
LAB_0055628f:
    iVar4 = ___acrt_IsValidLocaleName_4(pwVar8,(ushort *)param_1);
    if (iVar4 != 0) {
      uVar5 = get_default_code_page((ushort *)param_1);
      *pUVar2 = uVar5 & 0xffff;
      iVar4 = FUN_0055f860(local_1d8,0x83,(int)param_1,uVar11 + 1);
      if (iVar4 != 0) goto LAB_00556488;
      _expandlocale_locale_name_cache::commit_locale_name(local_1fc,param_1,uVar11 + 1);
      pwVar10 = local_1d8;
      goto LAB_0055640b;
    }
    pwVar8 = param_1;
    cVar3 = FUN_00556dc6(local_1d4,(ushort *)param_1);
    if ((cVar3 != '\0') &&
       (iVar4 = ___acrt_IsValidLocaleName_4(pwVar8,(ushort *)&local_b4), iVar4 != 0)) {
      if (local_d4 == 0) {
        uVar5 = get_default_code_page((ushort *)&local_b4);
      }
      else {
        uVar9 = (uint)local_d4;
        uVar5 = uVar9 + 0x20;
        if (0x19 < uVar9 - 0x41) {
          uVar5 = uVar9;
        }
        if (uVar5 == 0x75) {
          uVar9 = (uint)local_d2;
          uVar5 = uVar9 + 0x20;
          if (0x19 < uVar9 - 0x41) {
            uVar5 = uVar9;
          }
          if (uVar5 != 0x74) goto LAB_00556376;
          uVar9 = (uint)local_d0;
          uVar5 = uVar9 + 0x20;
          if (0x19 < uVar9 - 0x41) {
            uVar5 = uVar9;
          }
          if (((uVar5 != 0x66) || (local_ce != 0x38)) || (local_cc != 0)) goto LAB_00556376;
        }
        else {
LAB_00556376:
          if (((local_ce != 0x2d) || (local_cc != 0x38)) || (local_ca != 0)) goto LAB_0055646c;
        }
        uVar5 = 0xfde9;
      }
      pwVar10 = local_1d8;
      *pUVar2 = uVar5 & 0xffff;
      iVar4 = FUN_0055f860(local_1d8,0x83,(int)param_1,uVar11 + 1);
      if (iVar4 != 0) goto LAB_00556488;
      pwVar8 = &local_b4;
      do {
        wVar1 = *pwVar8;
        pwVar8 = pwVar8 + 1;
      } while (wVar1 != (wchar_t)local_1e4);
      _expandlocale_locale_name_cache::commit_locale_name
                (local_1fc,&local_b4,((int)pwVar8 - (int)local_b2 >> 1) + 1);
      goto LAB_0055640b;
    }
  }
LAB_0055646c:
  _expandlocale_locale_name_cache::~_expandlocale_locale_name_cache(local_1fc);
LAB_00556479:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

