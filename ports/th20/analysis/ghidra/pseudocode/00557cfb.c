/* Entry: 0x00557cfb; symbol: FUN_00557cfb; body bytes: 43 */

FARPROC __fastcall FUN_00557cfb(void *param_1)

{
  FARPROC pFVar1;
  
  if (DAT_005e6014 == (FARPROC)0xffffffff) {
    return (FARPROC)0x0;
  }
  pFVar1 = DAT_005e6014;
  if (DAT_005e6014 == (FARPROC)0x0) {
    pFVar1 = FUN_00557e53(param_1,5,"GetDateFormatEx",(int *)&DAT_00599d60,(int *)"GetDateFormatEx")
    ;
  }
  return pFVar1;
}

