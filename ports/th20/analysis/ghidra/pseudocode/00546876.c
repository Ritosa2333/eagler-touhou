/* Entry: 0x00546876; symbol: FUN_00546876; body bytes: 56 */

int FUN_00546876(void)

{
  DWORD dwErrCode;
  int iVar1;
  
  if (DAT_005b2630 == 0xffffffff) {
    return 0;
  }
  dwErrCode = GetLastError();
  iVar1 = ___vcrt_FlsGetValue(DAT_005b2630);
  SetLastError(dwErrCode);
  if (iVar1 == -1) {
    iVar1 = 0;
  }
  return iVar1;
}

