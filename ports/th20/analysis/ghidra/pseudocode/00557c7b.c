/* Entry: 0x00557c7b; symbol: FUN_00557c7b; body bytes: 42 */

FARPROC __fastcall FUN_00557c7b(void *param_1)

{
  FARPROC pFVar1;
  
  if (DAT_005e6000 == (FARPROC)0xffffffff) {
    return (FARPROC)0x0;
  }
  pFVar1 = DAT_005e6000;
  if (DAT_005e6000 == (FARPROC)0x0) {
    pFVar1 = FUN_00557e53(param_1,0,"AreFileApisANSI",(int *)&DAT_00599d00,(int *)"AreFileApisANSI")
    ;
  }
  return pFVar1;
}

