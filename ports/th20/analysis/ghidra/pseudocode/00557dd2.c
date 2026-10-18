/* Entry: 0x00557dd2; symbol: FUN_00557dd2; body bytes: 43 */

FARPROC __fastcall FUN_00557dd2(void *param_1)

{
  FARPROC pFVar1;
  
  if (DAT_005e604c == (FARPROC)0xffffffff) {
    return (FARPROC)0x0;
  }
  pFVar1 = DAT_005e604c;
  if (DAT_005e604c == (FARPROC)0x0) {
    pFVar1 = FUN_00557e53(param_1,0x13,"LCIDToLocaleName",(int *)&DAT_00599e00,
                          (int *)"LCIDToLocaleName");
  }
  return pFVar1;
}

