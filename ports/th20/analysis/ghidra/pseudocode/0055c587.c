/* Entry: 0x0055c587; symbol: FUN_0055c587; body bytes: 658 */

void __cdecl FUN_0055c587(wchar_t *param_1)

{
  wchar_t wVar1;
  wchar_t wVar2;
  long lVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  errno_t eVar6;
  uint uVar7;
  wchar_t *pwVar8;
  int iVar9;
  int *piVar10;
  uint *puVar11;
  wchar_t *pwVar12;
  bool bVar13;
  undefined4 local_10;
  uint local_c;
  long local_8;
  
  puVar4 = (undefined4 *)FUN_0055be4a();
  puVar5 = (undefined4 *)FUN_0055be50();
  local_8 = 0;
  local_c = 0;
  eVar6 = FID_conflict___get_dstbias(&local_8);
  if ((eVar6 == 0) && (eVar6 = FID_conflict___get_dstbias((long *)&local_c), eVar6 == 0)) {
    pwVar8 = DAT_005e58ac;
    pwVar12 = param_1;
    if (DAT_005e58ac != (wchar_t *)0x0) {
      do {
        wVar1 = *pwVar12;
        bVar13 = (ushort)wVar1 < (ushort)*pwVar8;
        if (wVar1 != *pwVar8) {
LAB_0055c607:
          uVar7 = -(uint)bVar13 | 1;
          goto LAB_0055c60c;
        }
        if (wVar1 == L'\0') break;
        wVar1 = pwVar12[1];
        bVar13 = (ushort)wVar1 < (ushort)pwVar8[1];
        if (wVar1 != pwVar8[1]) goto LAB_0055c607;
        pwVar8 = pwVar8 + 2;
        pwVar12 = pwVar12 + 2;
      } while (wVar1 != L'\0');
      uVar7 = 0;
LAB_0055c60c:
      if (uVar7 == 0) {
        return;
      }
    }
    pwVar8 = param_1;
    do {
      wVar1 = *pwVar8;
      pwVar8 = pwVar8 + 1;
    } while (wVar1 != L'\0');
    pwVar8 = (wchar_t *)FUN_005584c0(((int)pwVar8 - (int)(param_1 + 1) >> 1) * 2 + 2);
    if (pwVar8 != (wchar_t *)0x0) {
      FUN_00557ba0(DAT_005e58ac);
      pwVar12 = param_1;
      do {
        wVar1 = *pwVar12;
        pwVar12 = pwVar12 + 1;
      } while (wVar1 != L'\0');
      DAT_005e58ac = pwVar8;
      iVar9 = FUN_00548100(pwVar8,((int)pwVar12 - (int)(param_1 + 1) >> 1) + 1,(int)param_1);
      if (iVar9 != 0) goto LAB_0055c80f;
      _memset((void *)*puVar5,0,0x80);
      _memset((void *)puVar5[1],0,0x80);
      _memset((void *)*puVar4,0,0x40);
      _memset((void *)puVar4[1],0,0x40);
      tzset_env_copy_to_tzname(param_1,(wchar_t *)*puVar5,(char *)*puVar4,3);
      iVar9 = 3;
      do {
        if (*param_1 != L'\0') {
          param_1 = param_1 + 1;
        }
        iVar9 = iVar9 + -1;
      } while (iVar9 != 0);
      wVar1 = *param_1;
      pwVar8 = param_1 + 1;
      if (wVar1 != L'-') {
        pwVar8 = param_1;
      }
      uVar7 = FUN_0055171c(pwVar8,&local_10,10);
      local_8 = uVar7 * 0xe10;
      for (; (wVar2 = *pwVar8, wVar2 == L'+' || ((ushort)(wVar2 + L'￐') < 10));
          pwVar8 = pwVar8 + 1) {
      }
      if (wVar2 == L':') {
        pwVar8 = pwVar8 + 1;
        uVar7 = FUN_0055171c(pwVar8,&local_10,10);
        local_8 = local_8 + uVar7 * 0x3c;
        wVar2 = *pwVar8;
        while ((0x2f < (ushort)wVar2 && ((ushort)wVar2 < 0x3a))) {
          pwVar8 = pwVar8 + 1;
          wVar2 = *pwVar8;
        }
        if (wVar2 == L':') {
          pwVar8 = pwVar8 + 1;
          uVar7 = FUN_0055171c(pwVar8,&local_10,10);
          local_8 = local_8 + uVar7;
          wVar2 = *pwVar8;
          while ((0x2f < (ushort)wVar2 && ((ushort)wVar2 < 0x3a))) {
            pwVar8 = pwVar8 + 1;
            wVar2 = *pwVar8;
          }
        }
      }
      if (wVar1 == L'-') {
        local_8 = -local_8;
      }
      local_c = (uint)(*pwVar8 != L'\0');
      if (*pwVar8 != L'\0') {
        tzset_env_copy_to_tzname(pwVar8,(wchar_t *)puVar5[1],(char *)puVar4[1],3);
      }
      lVar3 = local_8;
      piVar10 = FUN_0055be44();
      uVar7 = local_c;
      *piVar10 = lVar3;
      puVar11 = FUN_0055be38();
      *puVar11 = uVar7;
    }
    FUN_00557ba0((LPVOID)0x0);
    return;
  }
LAB_0055c80f:
                    /* WARNING: Subroutine does not return */
  __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
}

