/* Entry: 0x00557d26; symbol: FUN_00557d26; body bytes: 43 */

FARPROC __fastcall FUN_00557d26(void *param_1)

{
  FARPROC pFVar1;
  
  if (DAT_005e6028 == (FARPROC)0xffffffff) {
    return (FARPROC)0x0;
  }
  pFVar1 = DAT_005e6028;
  if (DAT_005e6028 == (FARPROC)0x0) {
    pFVar1 = FUN_00557e53(param_1,10,"GetLocaleInfoEx",(int *)&DAT_00599d78,(int *)"GetLocaleInfoEx"
                         );
  }
  return pFVar1;
}

