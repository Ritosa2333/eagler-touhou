/* Entry: 0x00557ca5; symbol: FUN_00557ca5; body bytes: 43 */

FARPROC __fastcall FUN_00557ca5(void *param_1)

{
  FARPROC pFVar1;
  
  if (DAT_005e6004 == (FARPROC)0xffffffff) {
    return (FARPROC)0x0;
  }
  pFVar1 = DAT_005e6004;
  if (DAT_005e6004 == (FARPROC)0x0) {
    pFVar1 = FUN_00557e53(param_1,1,"CompareStringEx",(int *)&DAT_00599d14,(int *)"CompareStringEx")
    ;
  }
  return pFVar1;
}

