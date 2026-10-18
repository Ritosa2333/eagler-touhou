/* Entry: 0x0055dd91; symbol: FUN_0055dd91; body bytes: 42 */

void __cdecl
FUN_0055dd91(uint param_1,uint param_2,LPCSTR param_3,int param_4,LPWSTR param_5,int param_6)

{
  uint dwFlags;
  
  dwFlags = FUN_0055dcf9(param_1,param_2);
  MultiByteToWideChar(param_1,dwFlags,param_3,param_4,param_5,param_6);
  return;
}

