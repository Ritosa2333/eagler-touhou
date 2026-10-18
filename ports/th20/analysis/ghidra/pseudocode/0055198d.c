/* Entry: 0x0055198d; symbol: ____lc_codepage_func; body bytes: 37 */

/* Library Function - Single Match
    ____lc_codepage_func
   
   Library: Visual Studio 2019 Release */

UINT __cdecl ____lc_codepage_func(void)

{
  int iVar1;
  int local_8;
  
  iVar1 = FUN_005579c2();
  local_8 = *(int *)(iVar1 + 0x4c);
  ___acrt_update_locale_info(iVar1,&local_8);
  return *(UINT *)(local_8 + 8);
}

