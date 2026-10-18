/* Entry: 0x005581b5; symbol: ___acrt_IsValidLocaleName@4; body bytes: 56 */

/* Library Function - Single Match
    ___acrt_IsValidLocaleName@4
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __thiscall ___acrt_IsValidLocaleName_4(void *this,ushort *param_1)

{
  FARPROC pFVar1;
  LCID Locale;
  void *this_00;
  DWORD dwFlags;
  
  pFVar1 = FUN_00557da7(this);
  if (pFVar1 == (FARPROC)0x0) {
    dwFlags = 1;
    Locale = ___acrt_LocaleNameToLCID_8(this_00,param_1,0);
    IsValidLocale(Locale,dwFlags);
  }
  else {
    (*(code *)PTR_guard_check_icall_0056c37c)(param_1);
    (*pFVar1)();
  }
  return;
}

