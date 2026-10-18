/* Entry: 0x0040fe50; symbol: FUN_0040fe50; body bytes: 61 */

void __cdecl FUN_0040fe50(undefined1 *param_1,size_t param_2,char param_3)

{
  char cVar1;
  
  cVar1 = FUN_00411170();
  if (cVar1 == '\0') {
    _memset(param_1,(int)param_3,param_2);
  }
  else {
    FUN_0040fda0(param_1,param_2,param_3);
  }
  return;
}

