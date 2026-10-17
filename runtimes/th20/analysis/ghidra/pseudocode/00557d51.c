/* Entry: 0x00557d51; symbol: FUN_00557d51; body bytes: 43 */

FARPROC __fastcall FUN_00557d51(void *param_1)

{
  FARPROC pFVar1;
  
  if (DAT_005e6034 == (FARPROC)0xffffffff) {
    return (FARPROC)0x0;
  }
  pFVar1 = DAT_005e6034;
  if (DAT_005e6034 == (FARPROC)0x0) {
    pFVar1 = FUN_00557e53(param_1,0xd,"GetTimeFormatEx",(int *)&DAT_00599d90,
                          (int *)"GetTimeFormatEx");
  }
  return pFVar1;
}

