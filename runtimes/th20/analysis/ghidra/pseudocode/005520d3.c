/* Entry: 0x005520d3; symbol: FUN_005520d3; body bytes: 154 */

wchar_t * __cdecl FUN_005520d3(int param_1,byte *param_2)

{
  wchar_t *pwVar1;
  int iVar2;
  LPWSTR _Locale;
  byte *local_8;
  
  if (param_2 == (byte *)0x0) {
    pwVar1 = __wsetlocale(param_1,(wchar_t *)0x0);
  }
  else {
    pwVar1 = (wchar_t *)0x0;
    iVar2 = FUN_0055d6e2(&local_8,(LPWSTR)0x0,(byte *)0x0,param_2,(byte *)0x7fffffff);
    if ((iVar2 != 0) && ((iVar2 == 0x16 || (iVar2 == 0x22)))) {
LAB_00552163:
                    /* WARNING: Subroutine does not return */
      __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    _Locale = (LPWSTR)FUN_00557b40((uint)local_8,2);
    if (_Locale != (LPWSTR)0x0) {
      iVar2 = FUN_0055d6e2((undefined4 *)0x0,_Locale,local_8,param_2,(byte *)0xffffffff);
      if (iVar2 == 0) {
        pwVar1 = __wsetlocale(param_1,_Locale);
      }
      else if ((iVar2 == 0x16) || (iVar2 == 0x22)) goto LAB_00552163;
    }
    FUN_00557ba0(_Locale);
  }
  return pwVar1;
}

