/* Entry: 0x00453220; symbol: FUN_00453220; body bytes: 157 */

/* WARNING: Function: __alloca_probe replaced with injection: alloca_probe */

void __fastcall FUN_00453220(void *param_1)

{
  bool bVar1;
  LPCSTR lpMultiByteStr;
  int cbMultiByte;
  WCHAR *lpWideCharStr;
  int cchWideChar;
  WCHAR local_3e88 [8000];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  bVar1 = FUN_00454450((int)param_1);
  if (!bVar1) {
    FUN_00454150(param_1,L"---------------------------------------------------------- \r\n");
    if (*(char *)((int)param_1 + 0x1c) != '\0') {
      cchWideChar = 8000;
      lpWideCharStr = local_3e88;
      cbMultiByte = -1;
      lpMultiByteStr = (LPCSTR)FUN_0044baf0((int)param_1);
      MultiByteToWideChar(0x3a4,0,lpMultiByteStr,cbMultiByte,lpWideCharStr,cchWideChar);
      MessageBoxW((HWND)0x0,local_3e88,L"log",0x10);
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

