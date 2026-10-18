/* Entry: 0x004ecb10; symbol: FUN_004ecb10; body bytes: 54 */

undefined1 __cdecl FUN_004ecb10(int param_1)

{
  undefined1 uVar1;
  
  if (param_1 < -0x80) {
    uVar1 = 0;
  }
  else if (param_1 < 0x80) {
    uVar1 = 1;
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}

