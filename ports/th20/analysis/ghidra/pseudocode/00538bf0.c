/* Entry: 0x00538bf0; symbol: FUN_00538bf0; body bytes: 43 */

DWORD __fastcall FUN_00538bf0(int param_1)

{
  DWORD DVar1;
  
  if (*(int *)(param_1 + 4) == -1) {
    DVar1 = 0;
  }
  else {
    DVar1 = SetFilePointer(*(HANDLE *)(param_1 + 4),0,(PLONG)0x0,1);
  }
  return DVar1;
}

