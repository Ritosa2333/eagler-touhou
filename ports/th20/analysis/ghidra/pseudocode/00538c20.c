/* Entry: 0x00538c20; symbol: FUN_00538c20; body bytes: 39 */

DWORD __fastcall FUN_00538c20(int param_1)

{
  DWORD DVar1;
  
  if (*(int *)(param_1 + 4) == -1) {
    DVar1 = 0;
  }
  else {
    DVar1 = GetFileSize(*(HANDLE *)(param_1 + 4),(LPDWORD)0x0);
  }
  return DVar1;
}

