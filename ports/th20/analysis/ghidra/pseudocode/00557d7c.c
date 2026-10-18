/* Entry: 0x00557d7c; symbol: FUN_00557d7c; body bytes: 43 */

FARPROC __fastcall FUN_00557d7c(void *param_1)

{
  FARPROC pFVar1;
  
  if (DAT_005e6038 == (FARPROC)0xffffffff) {
    return (FARPROC)0x0;
  }
  pFVar1 = DAT_005e6038;
  if (DAT_005e6038 == (FARPROC)0x0) {
    pFVar1 = FUN_00557e53(param_1,0xe,"GetUserDefaultLocaleName",(int *)&DAT_00599da8,
                          (int *)"GetUserDefaultLocaleName");
  }
  return pFVar1;
}

