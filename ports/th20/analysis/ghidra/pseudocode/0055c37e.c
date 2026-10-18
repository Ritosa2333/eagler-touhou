/* Entry: 0x0055c37e; symbol: FUN_0055c37e; body bytes: 330 */

void __cdecl
FUN_0055c37e(int param_1,int param_2,uint param_3,int param_4,int param_5,int param_6,int param_7,
            int param_8,int param_9,int param_10,int param_11)

{
  int iVar1;
  int iVar2;
  errno_t eVar3;
  int iVar4;
  int *piVar5;
  long local_10;
  int *local_c;
  char local_5;
  
  local_10 = 0;
  iVar1 = FUN_00552818(param_3);
  local_5 = (char)iVar1;
  if (param_2 == 1) {
    local_c = &DAT_005a25e0 + param_4;
    if (local_5 == '\0') {
      iVar1 = *(int *)(&DAT_005a25dc + param_4 * 4);
    }
    else {
      iVar1 = *(int *)(&DAT_005a2610 + param_4 * 4);
    }
    iVar2 = __crt_time_elapsed_leap_years<>(param_3);
    iVar4 = (int)(iVar2 + iVar1 + 1 + param_3 * 0x16d + -0x63ca) % 7;
    iVar2 = (param_5 * 7 - iVar4) + iVar1 + 1 + param_6;
    iVar1 = iVar2 + -7;
    if (param_6 < iVar4) {
      iVar1 = iVar2;
    }
    piVar5 = (int *)(&DAT_005a2614 + param_4 * 4);
    if (local_5 == '\0') {
      piVar5 = local_c;
    }
    if ((param_5 == 5) && (*piVar5 < iVar1)) {
      iVar1 = iVar1 + -7;
    }
  }
  else {
    if (local_5 == '\0') {
      iVar1 = *(int *)(&DAT_005a25dc + param_4 * 4);
    }
    else {
      iVar1 = *(int *)(&DAT_005a2610 + param_4 * 4);
    }
    iVar1 = iVar1 + param_7;
  }
  iVar2 = param_11 + ((param_8 * 0x3c + param_9) * 0x3c + param_10) * 1000;
  if (param_1 == 0) {
    DAT_005b29e0 = param_3;
    DAT_005b29e4 = iVar1;
    DAT_005b29e8 = iVar2;
  }
  else {
    DAT_005b29f0 = iVar1;
    DAT_005b29f4 = iVar2;
    eVar3 = FID_conflict___get_dstbias(&local_10);
    if (eVar3 != 0) {
                    /* WARNING: Subroutine does not return */
      __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    DAT_005b29f4 = DAT_005b29f4 + local_10 * 1000;
    DAT_005b29ec = param_3;
    if (DAT_005b29f4 < 0) {
      DAT_005b29f4 = DAT_005b29f4 + 86400000;
      DAT_005b29f0 = DAT_005b29f0 + -1;
    }
    else if (86399999 < DAT_005b29f4) {
      DAT_005b29f4 = DAT_005b29f4 + -86400000;
      DAT_005b29f0 = DAT_005b29f0 + 1;
    }
  }
  return;
}

