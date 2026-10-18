/* Entry: 0x005519b2; symbol: ____lc_locale_name_func; body bytes: 39 */

/* Library Function - Single Match
    ____lc_locale_name_func
   
   Library: Visual Studio 2019 Release */

wchar_t ** __cdecl ____lc_locale_name_func(void)

{
  int iVar1;
  int local_8;
  
  iVar1 = FUN_005579c2();
  local_8 = *(int *)(iVar1 + 0x4c);
  ___acrt_update_locale_info(iVar1,&local_8);
  return (wchar_t **)(local_8 + 0xa0);
}

