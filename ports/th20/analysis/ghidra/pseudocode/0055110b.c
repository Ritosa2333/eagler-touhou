/* Entry: 0x0055110b; symbol: FUN_0055110b; body bytes: 656 */

int __cdecl FUN_0055110b(tm *param_1,uint *param_2)

{
  __acrt_ptd *p_Var1;
  errno_t eVar2;
  uint uVar3;
  int iVar4;
  int iVar5;
  uint uVar6;
  tm *ptVar7;
  uint uVar8;
  bool bVar9;
  undefined8 uVar10;
  longlong lVar11;
  uint local_18;
  int local_14;
  uint local_10;
  long local_c;
  uint local_8;
  
  if (param_1 == (tm *)0x0) {
    p_Var1 = FUN_005516c1();
    *(undefined4 *)p_Var1 = 0x16;
    FUN_005480bc();
    return 0x16;
  }
  ptVar7 = param_1;
  for (iVar5 = 9; iVar5 != 0; iVar5 = iVar5 + -1) {
    ptVar7->tm_sec = -1;
    ptVar7 = (tm *)&ptVar7->tm_min;
  }
  if (param_2 == (uint *)0x0) {
    p_Var1 = FUN_005516c1();
    *(undefined4 *)p_Var1 = 0x16;
    FUN_005480bc();
    return 0x16;
  }
  if ((((int)param_2[1] < 1) && ((int)param_2[1] < 0)) ||
     ((6 < (int)param_2[1] && ((7 < (int)param_2[1] || (0x93582aff < *param_2)))))) {
    p_Var1 = FUN_005516c1();
    *(undefined4 *)p_Var1 = 0x16;
    return 0x16;
  }
  ___tzset();
  local_c = 0;
  local_10 = 0;
  local_8 = 0;
  eVar2 = FID_conflict___get_dstbias(&local_c);
  if (((eVar2 != 0) || (eVar2 = FID_conflict___get_dstbias((long *)&local_10), eVar2 != 0)) ||
     (eVar2 = FID_conflict___get_dstbias((long *)&local_8), eVar2 != 0)) {
                    /* WARNING: Subroutine does not return */
    __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
  }
  uVar6 = *param_2;
  uVar3 = (param_2[1] - 1) + (uint)(0x3f480 < uVar6);
  if ((7 < uVar3) || ((6 < uVar3 && (0x935041fd < uVar6 - 0x3f481)))) {
    iVar5 = FUN_0055c118(&param_1->tm_sec,param_2);
    if (iVar5 != 0) {
      return iVar5;
    }
    uVar6 = param_1->tm_sec;
    uVar3 = local_8;
    if ((local_c != 0) && (iVar5 = __isindst(param_1), uVar3 = local_8, iVar5 != 0)) {
      param_1->tm_isdst = 1;
      uVar3 = local_8 + local_10;
    }
    uVar8 = uVar6 - uVar3;
    uVar6 = (((int)uVar6 >> 0x1f) - ((int)uVar3 >> 0x1f)) - (uint)(uVar6 < uVar3);
    uVar10 = __allrem(uVar8,uVar6,0x3c,0);
    iVar5 = (int)uVar10;
    param_1->tm_sec = iVar5;
    if (iVar5 < 0) {
      bVar9 = 0x3b < uVar8;
      uVar8 = uVar8 - 0x3c;
      param_1->tm_sec = iVar5 + 0x3c;
      uVar6 = (uVar6 - 1) + (uint)bVar9;
    }
    lVar11 = __alldiv(uVar8,uVar6,0x3c,0);
    lVar11 = lVar11 + param_1->tm_min;
    uVar10 = __allrem((uint)lVar11,(uint)((ulonglong)lVar11 >> 0x20),0x3c,0);
    iVar5 = (int)uVar10;
    param_1->tm_min = iVar5;
    if (iVar5 < 0) {
      param_1->tm_min = iVar5 + 0x3c;
      lVar11 = lVar11 + -0x3c;
    }
    lVar11 = __alldiv((uint)lVar11,(uint)((ulonglong)lVar11 >> 0x20),0x3c,0);
    lVar11 = lVar11 + param_1->tm_hour;
    uVar10 = __allrem((uint)lVar11,(uint)((ulonglong)lVar11 >> 0x20),0x18,0);
    iVar5 = (int)uVar10;
    param_1->tm_hour = iVar5;
    if (iVar5 < 0) {
      param_1->tm_hour = iVar5 + 0x18;
      lVar11 = lVar11 + -0x18;
    }
    lVar11 = __alldiv((uint)lVar11,(uint)((ulonglong)lVar11 >> 0x20),0x18,0);
    iVar4 = (int)((ulonglong)lVar11 >> 0x20);
    iVar5 = (int)lVar11;
    if (-1 < lVar11) {
      if ((iVar4 != 0 && -1 < lVar11) || (iVar5 != 0)) {
        param_1->tm_mday = param_1->tm_mday + iVar5;
        param_1->tm_yday = param_1->tm_yday + iVar5;
        param_1->tm_wday = (param_1->tm_wday + iVar5) % 7;
        return 0;
      }
      if (iVar4 != 0 && -1 < lVar11) {
        return 0;
      }
      if (-1 < lVar11) {
        return 0;
      }
    }
    param_1->tm_wday = (param_1->tm_wday + 7 + iVar5) % 7;
    iVar4 = param_1->tm_mday + iVar5;
    iVar5 = param_1->tm_yday + iVar5;
    param_1->tm_mday = iVar4;
    if (iVar4 < 1) {
      param_1->tm_mon = 0xb;
      param_1->tm_year = param_1->tm_year + -1;
      param_1->tm_mday = iVar4 + 0x1f;
      param_1->tm_yday = iVar5 + 0x16d;
    }
    else {
      param_1->tm_yday = iVar5;
    }
    return 0;
  }
  local_18 = uVar6 - local_8;
  local_14 = (param_2[1] - ((int)local_8 >> 0x1f)) - (uint)(uVar6 < local_8);
  iVar5 = FUN_0055c118(&param_1->tm_sec,&local_18);
  if (iVar5 != 0) {
    return iVar5;
  }
  if (local_c == 0) {
    return 0;
  }
  iVar5 = __isindst(param_1);
  if (iVar5 == 0) {
    return 0;
  }
  bVar9 = local_18 < local_10;
  local_18 = local_18 - local_10;
  local_14 = (local_14 - ((int)local_10 >> 0x1f)) - (uint)bVar9;
  iVar5 = FUN_0055c118(&param_1->tm_sec,&local_18);
  if (iVar5 == 0) {
    param_1->tm_isdst = 1;
    return 0;
  }
  return iVar5;
}

