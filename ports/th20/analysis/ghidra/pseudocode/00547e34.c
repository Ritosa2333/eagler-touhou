/* Entry: 0x00547e34; symbol: FUN_00547e34; body bytes: 41 */

undefined4 __fastcall FUN_00547e34(int param_1)

{
  DWORD dwErrCode;
  
  if (*(char *)(param_1 + 8) == '\0') {
    dwErrCode = GetLastError();
    *(undefined4 *)(param_1 + 4) = 0;
    *(undefined1 *)(param_1 + 8) = 1;
    SetLastError(dwErrCode);
    return 0;
  }
  return *(undefined4 *)(param_1 + 4);
}

