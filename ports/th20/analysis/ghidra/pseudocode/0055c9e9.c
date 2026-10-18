/* Entry: 0x0055c9e9; symbol: tzset_os_copy_to_tzname; body bytes: 65 */

/* Library Function - Single Match
    void __cdecl tzset_os_copy_to_tzname(wchar_t const * const,wchar_t * const,char * const,unsigned
   int)
   
   Library: Visual Studio 2019 Release */

void __cdecl tzset_os_copy_to_tzname(wchar_t *param_1,wchar_t *param_2,char *param_3,uint param_4)

{
  int iVar1;
  
  iVar1 = FUN_0055f860(param_2,0x40,(int)param_1,0x20);
  if (iVar1 == 0) {
    FUN_0055de4b(param_4,0,param_1,-1,param_3,0x40,(LPCSTR)0x0,(LPBOOL)0x0);
    return;
  }
                    /* WARNING: Subroutine does not return */
  __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
}

