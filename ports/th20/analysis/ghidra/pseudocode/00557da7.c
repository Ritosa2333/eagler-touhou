/* Entry: 0x00557da7; symbol: FUN_00557da7; body bytes: 43 */

FARPROC __fastcall FUN_00557da7(void *param_1)

{
  FARPROC pFVar1;
  
  if (DAT_005e6044 == (FARPROC)0xffffffff) {
    return (FARPROC)0x0;
  }
  pFVar1 = DAT_005e6044;
  if (DAT_005e6044 == (FARPROC)0x0) {
    pFVar1 = FUN_00557e53(param_1,0x11,"IsValidLocaleName",(int *)&DAT_00599dcc,
                          (int *)"IsValidLocaleName");
  }
  return pFVar1;
}

