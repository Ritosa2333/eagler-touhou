/* Entry: 0x0055c540; symbol: tzset_env_copy_to_tzname; body bytes: 70 */

/* Library Function - Single Match
    void __cdecl tzset_env_copy_to_tzname(wchar_t const * const,wchar_t * const,char *
   const,unsigned int)
   
   Library: Visual Studio 2019 Release */

void __cdecl tzset_env_copy_to_tzname(wchar_t *param_1,wchar_t *param_2,char *param_3,uint param_4)

{
  LPBOOL pBVar1;
  uint uVar2;
  int iVar3;
  LPBOOL pBVar4;
  LPBOOL pBVar5;
  
  pBVar1 = (LPBOOL)FUN_0055f860(param_2,0x40,(int)param_1,param_4);
  if (pBVar1 == (LPBOOL)0x0) {
    iVar3 = 0x3f;
    pBVar4 = pBVar1;
    pBVar5 = pBVar1;
    uVar2 = __acrt_get_utf8_acp_compatibility_codepage();
    FUN_0055de4b(uVar2,(uint)pBVar1,param_2,param_4,param_3,iVar3,(LPCSTR)pBVar4,pBVar5);
    return;
  }
                    /* WARNING: Subroutine does not return */
  __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
}

