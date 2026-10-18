/* Entry: 0x005578cf; symbol: FUN_005578cf; body bytes: 168 */

__acrt_ptd * FUN_005578cf(void)

{
  DWORD dwErrCode;
  int iVar1;
  __acrt_ptd *p_Var2;
  
  dwErrCode = GetLastError();
  iVar1 = FUN_00558122();
  if (iVar1 == 0) {
    SetLastError(dwErrCode);
    return (__acrt_ptd *)0x0;
  }
  p_Var2 = (__acrt_ptd *)FUN_00557b40(1,0x364);
  if (p_Var2 == (__acrt_ptd *)0x0) {
    FUN_00558122();
    FUN_00557ba0((LPVOID)0x0);
    SetLastError(dwErrCode);
    p_Var2 = (__acrt_ptd *)0x0;
  }
  else {
    iVar1 = FUN_00558122();
    if (iVar1 == 0) {
      FUN_00558122();
      FUN_00557ba0(p_Var2);
      p_Var2 = (__acrt_ptd *)0x0;
    }
    else {
      construct_ptd(p_Var2,(__crt_locale_data **)&DAT_005e5664);
      FUN_00557ba0((LPVOID)0x0);
    }
    SetLastError(dwErrCode);
  }
  return p_Var2;
}

