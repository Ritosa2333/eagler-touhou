/* Entry: 0x005467e4; symbol: ___vcrt_getptd_noexit; body bytes: 146 */

/* Library Function - Single Match
    ___vcrt_getptd_noexit
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

LPVOID ___vcrt_getptd_noexit(void)

{
  DWORD dwErrCode;
  LPVOID pvVar1;
  int iVar2;
  LPVOID pvVar3;
  LPVOID pvVar4;
  
  if (DAT_005b2630 == 0xffffffff) {
    return (LPVOID)0x0;
  }
  dwErrCode = GetLastError();
  pvVar1 = (LPVOID)___vcrt_FlsGetValue(DAT_005b2630);
  if (pvVar1 == (LPVOID)0xffffffff) {
LAB_00546824:
    pvVar1 = (LPVOID)0x0;
    goto LAB_0054686a;
  }
  if (pvVar1 != (LPVOID)0x0) goto LAB_0054686a;
  iVar2 = ___vcrt_FlsSetValue(DAT_005b2630,(LPVOID)0xffffffff);
  if (iVar2 == 0) goto LAB_00546824;
  pvVar3 = (LPVOID)FUN_00551400(1,0x28);
  if (pvVar3 == (LPVOID)0x0) {
LAB_0054684c:
    ___vcrt_FlsSetValue(DAT_005b2630,(LPVOID)0x0);
    pvVar1 = (LPVOID)0x0;
    pvVar4 = pvVar3;
  }
  else {
    iVar2 = ___vcrt_FlsSetValue(DAT_005b2630,pvVar3);
    if (iVar2 == 0) goto LAB_0054684c;
    pvVar4 = (LPVOID)0x0;
    pvVar1 = pvVar3;
  }
  thunk_FUN_00557ba0(pvVar4);
LAB_0054686a:
  SetLastError(dwErrCode);
  return pvVar1;
}

