/* Entry: 0x00557dfd; symbol: FUN_00557dfd; body bytes: 43 */

FARPROC __fastcall FUN_00557dfd(void *param_1)

{
  FARPROC pFVar1;
  
  if (DAT_005e6048 == (FARPROC)0xffffffff) {
    return (FARPROC)0x0;
  }
  pFVar1 = DAT_005e6048;
  if (DAT_005e6048 == (FARPROC)0x0) {
    pFVar1 = FUN_00557e53(param_1,0x12,"LCMapStringEx",(int *)&DAT_00599de8,(int *)"LCMapStringEx");
  }
  return pFVar1;
}

