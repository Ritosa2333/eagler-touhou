/* Entry: 0x0055de4b; symbol: FUN_0055de4b; body bytes: 106 */

void __cdecl
FUN_0055de4b(uint param_1,uint param_2,LPCWSTR param_3,int param_4,LPSTR param_5,int param_6,
            LPCSTR param_7,LPBOOL param_8)

{
  bool bVar1;
  uint dwFlags;
  UINT CodePage;
  LPBOOL lpUsedDefaultChar;
  
  if ((param_1 == 65000) || (param_1 == 0xfde9)) {
    bVar1 = true;
  }
  else {
    bVar1 = false;
  }
  dwFlags = FUN_0055ddbb(param_1,param_2);
  lpUsedDefaultChar = param_8;
  if (bVar1) {
    lpUsedDefaultChar = (LPBOOL)0x0;
  }
  if ((bVar1) && (param_7 = (LPCSTR)0x0, param_8 != (LPBOOL)0x0)) {
    *param_8 = 0;
  }
  WideCharToMultiByte(CodePage,dwFlags,param_3,param_4,param_5,param_6,param_7,lpUsedDefaultChar);
  return;
}

