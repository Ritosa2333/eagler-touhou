/* Entry: 0x0055822c; symbol: ___acrt_LCMapStringEx@36; body bytes: 93 */

/* Library Function - Single Match
    ___acrt_LCMapStringEx@36
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

void __thiscall
___acrt_LCMapStringEx_36
          (void *this,ushort *param_1,DWORD param_2,LPCWSTR param_3,int param_4,LPWSTR param_5,
          int param_6,undefined4 param_7,undefined4 param_8,undefined4 param_9)

{
  FARPROC pFVar1;
  LCID Locale;
  void *this_00;
  
  pFVar1 = FUN_00557dfd(this);
  if (pFVar1 == (FARPROC)0x0) {
    Locale = ___acrt_LocaleNameToLCID_8(this_00,param_1,0);
    LCMapStringW(Locale,param_2,param_3,param_4,param_5,param_6);
  }
  else {
    (*(code *)PTR_guard_check_icall_0056c37c)
              (param_1,param_2,param_3,param_4,param_5,param_6,param_7,param_8,param_9);
    (*pFVar1)();
  }
  return;
}

