/* Entry: 0x00557cd0; symbol: FUN_00557cd0; body bytes: 43 */

FARPROC __fastcall FUN_00557cd0(void *param_1)

{
  FARPROC pFVar1;
  
  if (DAT_005e6008 == (FARPROC)0xffffffff) {
    return (FARPROC)0x0;
  }
  pFVar1 = DAT_005e6008;
  if (DAT_005e6008 == (FARPROC)0x0) {
    pFVar1 = FUN_00557e53(param_1,2,"EnumSystemLocalesEx",(int *)&DAT_00599d2c,
                          (int *)"EnumSystemLocalesEx");
  }
  return pFVar1;
}

