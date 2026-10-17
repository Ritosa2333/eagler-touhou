/* Entry: 0x0055f0ee; symbol: FUN_0055f0ee; body bytes: 160 */

LPWCH FUN_0055f0ee(void)

{
  LPWCH pWVar1;
  wchar_t *pwVar2;
  int iVar3;
  SIZE_T SVar4;
  LPWCH pWVar5;
  
  pWVar1 = GetEnvironmentStringsW();
  pWVar5 = pWVar1;
  if (pWVar1 != (LPWCH)0x0) {
    pwVar2 = find_end_of_double_null_terminated_sequence(pWVar1);
    iVar3 = (int)pwVar2 - (int)pWVar1 >> 1;
    SVar4 = FUN_0055de4b(0,0,pWVar1,iVar3,(LPSTR)0x0,0,(LPCSTR)0x0,(LPBOOL)0x0);
    if (SVar4 == 0) {
      FreeEnvironmentStringsW(pWVar1);
      pWVar5 = (LPWCH)0x0;
    }
    else {
      pWVar5 = (LPWCH)FUN_005584c0(SVar4);
      if (pWVar5 == (LPWCH)0x0) {
        FUN_00557ba0((LPVOID)0x0);
        FreeEnvironmentStringsW(pWVar1);
        pWVar5 = (LPWCH)0x0;
      }
      else {
        iVar3 = FUN_0055de4b(0,0,pWVar1,iVar3,(LPSTR)pWVar5,SVar4,(LPCSTR)0x0,(LPBOOL)0x0);
        if (iVar3 == 0) {
          FUN_00557ba0(pWVar5);
          pWVar5 = (LPWCH)0x0;
        }
        else {
          FUN_00557ba0((LPVOID)0x0);
        }
        FreeEnvironmentStringsW(pWVar1);
      }
    }
  }
  return pWVar5;
}

