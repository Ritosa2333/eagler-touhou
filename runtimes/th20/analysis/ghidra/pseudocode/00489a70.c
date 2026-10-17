/* Entry: 0x00489a70; symbol: FUN_00489a70; body bytes: 67 */

int __cdecl FUN_00489a70(int param_1,int param_2)

{
  char cVar1;
  
  while( true ) {
    if (param_1 == param_2) {
      return param_1;
    }
    _Dest_val<>(&stack0x00000010,param_1);
    cVar1 = FUN_0048b1c0();
    if (cVar1 != '\0') break;
    param_1 = param_1 + 4;
  }
  return param_1;
}

