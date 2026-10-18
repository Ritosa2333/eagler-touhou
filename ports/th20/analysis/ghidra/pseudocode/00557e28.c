/* Entry: 0x00557e28; symbol: FUN_00557e28; body bytes: 43 */

FARPROC __fastcall FUN_00557e28(void *param_1)

{
  FARPROC pFVar1;
  
  if (DAT_005e6050 == (FARPROC)0xffffffff) {
    return (FARPROC)0x0;
  }
  pFVar1 = DAT_005e6050;
  if (DAT_005e6050 == (FARPROC)0x0) {
    pFVar1 = FUN_00557e53(param_1,0x14,"LocaleNameToLCID",(int *)&DAT_00599e1c,
                          (int *)"LocaleNameToLCID");
  }
  return pFVar1;
}

