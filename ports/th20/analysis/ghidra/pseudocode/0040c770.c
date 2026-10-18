/* Entry: 0x0040c770; symbol: FUN_0040c770; body bytes: 76 */

uint __cdecl FUN_0040c770(uint param_1)

{
  char cVar1;
  uint uVar2;
  
  if (param_1 == 0) {
    uVar2 = 0;
  }
  else {
    cVar1 = FUN_00411170();
    if (cVar1 == '\0') {
      if (param_1 < 0x1000) {
        uVar2 = FUN_0040eca0(param_1);
      }
      else {
        uVar2 = FUN_0040c920(param_1);
      }
    }
    else {
      uVar2 = FUN_0040eca0(param_1);
    }
  }
  return uVar2;
}

