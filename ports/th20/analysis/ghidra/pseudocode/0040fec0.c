/* Entry: 0x0040fec0; symbol: FUN_0040fec0; body bytes: 61 */

void __cdecl FUN_0040fec0(wchar_t *param_1,size_t param_2,wchar_t param_3)

{
  char cVar1;
  
  cVar1 = FUN_00411170();
  if (cVar1 == '\0') {
    _wmemset(param_1,param_3,param_2);
  }
  else {
    FUN_0040fdf0(param_1,param_2,param_3);
  }
  return;
}

