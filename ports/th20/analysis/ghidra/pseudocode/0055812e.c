/* Entry: 0x0055812e; symbol: ___acrt_GetLocaleInfoEx@16; body bytes: 63 */

/* Library Function - Single Match
    ___acrt_GetLocaleInfoEx@16
   
   Library: Visual Studio 2019 Release */

void __thiscall
___acrt_GetLocaleInfoEx_16(void *this,ushort *param_1,LCTYPE param_2,LPWSTR param_3,int param_4)

{
  FARPROC pFVar1;
  LCID Locale;
  void *this_00;
  
  pFVar1 = FUN_00557d26(this);
  if (pFVar1 == (FARPROC)0x0) {
    Locale = ___acrt_LocaleNameToLCID_8(this_00,param_1,0);
    GetLocaleInfoW(Locale,param_2,param_3,param_4);
  }
  else {
    (*(code *)PTR_guard_check_icall_0056c37c)(param_1);
    (*pFVar1)();
  }
  return;
}

