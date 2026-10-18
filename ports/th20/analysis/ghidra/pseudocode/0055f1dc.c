/* Entry: 0x0055f1dc; symbol: FUN_0055f1dc; body bytes: 649 */

undefined4 __cdecl FUN_0055f1dc(wchar_t *param_1,int param_2)

{
  uint uVar1;
  wchar_t wVar2;
  wchar_t wVar3;
  __acrt_ptd *p_Var4;
  wchar_t *pwVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  int *piVar9;
  LPCWSTR lpName;
  LPCWSTR lpValue;
  BOOL BVar10;
  undefined4 uVar11;
  wchar_t *pwVar12;
  int *local_8;
  
  if (param_1 == (wchar_t *)0x0) {
    p_Var4 = FUN_005516c1();
    *(undefined4 *)p_Var4 = 0x16;
    return 0xffffffff;
  }
  pwVar5 = (wchar_t *)FUN_0054628e((ushort *)param_1,0x3d);
  if ((pwVar5 == (wchar_t *)0x0) || (pwVar5 == param_1)) {
    p_Var4 = FUN_005516c1();
    *(undefined4 *)p_Var4 = 0x16;
    FUN_00557ba0(param_1);
    return 0xffffffff;
  }
  wVar2 = pwVar5[1];
  ensure_current_environment_is_not_initial_environment_nolock<wchar_t>();
  uVar11 = 0;
  local_8 = DAT_005e55ac;
  if (DAT_005e55ac == (int *)0x0) {
    if ((param_2 == 0) || (DAT_005e55a8 == (LPVOID)0x0)) {
      if (wVar2 == L'\0') goto LAB_0055f427;
      if (DAT_005e55a8 == (LPVOID)0x0) {
        DAT_005e55a8 = FUN_00557b40(1,4);
        FUN_00557ba0((LPVOID)0x0);
        if (DAT_005e55a8 == (LPVOID)0x0) goto LAB_0055f267;
        local_8 = DAT_005e55ac;
        if (DAT_005e55ac != (int *)0x0) goto LAB_0055f2db;
      }
      DAT_005e55ac = (int *)FUN_00557b40(1,4);
      FUN_00557ba0((LPVOID)0x0);
      if (DAT_005e55ac != (int *)0x0) goto LAB_0055f2d7;
    }
    else {
      iVar6 = thunk_FUN_00553148();
      if (iVar6 == 0) {
        p_Var4 = FUN_005516c1();
        *(undefined4 *)p_Var4 = 0x16;
      }
      else {
        ensure_current_environment_is_not_initial_environment_nolock<wchar_t>();
LAB_0055f2d7:
        local_8 = DAT_005e55ac;
        if (DAT_005e55ac != (int *)0x0) goto LAB_0055f2db;
      }
    }
  }
  else {
LAB_0055f2db:
    uVar7 = (int)pwVar5 - (int)param_1 >> 1;
    DAT_005e55ac = local_8;
    uVar8 = find_in_environment_nolock<wchar_t>(param_1,uVar7);
    if ((-1 < (int)uVar8) && (*local_8 != 0)) {
      FUN_00557ba0((LPVOID)local_8[uVar8]);
      if (wVar2 == L'\0') {
        for (; local_8[uVar8] != 0; uVar8 = uVar8 + 1) {
          local_8[uVar8] = local_8[uVar8 + 1];
        }
        piVar9 = (int *)__recalloc_base(local_8,uVar8,4);
        FUN_00557ba0((LPVOID)0x0);
        pwVar5 = param_1;
        if (piVar9 != (int *)0x0) {
LAB_0055f39c:
          DAT_005e55ac = piVar9;
        }
      }
      else {
        pwVar5 = (wchar_t *)0x0;
        local_8[uVar8] = (int)param_1;
      }
      if (param_2 == 0) {
LAB_0055f437:
        FUN_00557ba0(pwVar5);
        return 0;
      }
      pwVar12 = param_1;
      do {
        wVar3 = *pwVar12;
        pwVar12 = pwVar12 + 1;
      } while (wVar3 != L'\0');
      uVar8 = ((int)pwVar12 - (int)(param_1 + 1) >> 1) + 2;
      lpName = (LPCWSTR)FUN_00557b40(uVar8,2);
      if (lpName == (LPCWSTR)0x0) {
        FUN_00557ba0((LPVOID)0x0);
        param_1 = pwVar5;
      }
      else {
        iVar6 = FUN_00548100(lpName,uVar8,(int)param_1);
        if (iVar6 != 0) {
                    /* WARNING: Subroutine does not return */
          __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
        }
        lpName[uVar7] = L'\0';
        lpValue = lpName + uVar7 + 1;
        if (wVar2 == L'\0') {
          lpValue = (LPCWSTR)0x0;
        }
        BVar10 = SetEnvironmentVariableW(lpName,lpValue);
        if (BVar10 != 0) {
          FUN_00557ba0(lpName);
          goto LAB_0055f437;
        }
        p_Var4 = FUN_005516c1();
        *(undefined4 *)p_Var4 = 0x2a;
        FUN_00557ba0(lpName);
        uVar11 = 0xffffffff;
        param_1 = pwVar5;
      }
      goto LAB_0055f427;
    }
    if (wVar2 == L'\0') goto LAB_0055f427;
    uVar1 = -uVar8 + 2;
    if ((-uVar8 <= uVar1) && (uVar1 < 0x3fffffff)) {
      piVar9 = (int *)__recalloc_base(local_8,uVar1,4);
      FUN_00557ba0((LPVOID)0x0);
      if (piVar9 != (int *)0x0) {
        piVar9[-uVar8] = (int)param_1;
        piVar9[1 - uVar8] = 0;
        pwVar5 = (wchar_t *)0x0;
        goto LAB_0055f39c;
      }
    }
  }
LAB_0055f267:
  uVar11 = 0xffffffff;
LAB_0055f427:
  FUN_00557ba0(param_1);
  return uVar11;
}

