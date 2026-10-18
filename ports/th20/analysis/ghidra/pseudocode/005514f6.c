/* Entry: 0x005514f6; symbol: _localeconv; body bytes: 40 */

/* Library Function - Single Match
    _localeconv
   
   Library: Visual Studio 2019 Release */

lconv * __cdecl _localeconv(void)

{
  int iVar1;
  int local_8;
  
  iVar1 = FUN_005579c2();
  local_8 = *(int *)(iVar1 + 0x4c);
  ___acrt_update_locale_info(iVar1,&local_8);
  return *(lconv **)(local_8 + 0x88);
}

