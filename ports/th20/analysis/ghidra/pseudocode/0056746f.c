/* Entry: 0x0056746f; symbol: FUN_0056746f; body bytes: 119 */

int __cdecl FUN_0056746f(int param_1)

{
  uint uVar1;
  
  if ((DAT_005e5288 < 1) || ((param_1 != -0x3ffffd4c && (param_1 != -0x3ffffd4b)))) {
    return param_1;
  }
  uVar1 = MXCSR ^ 0x3f;
  if ((uVar1 & 0x81) != 0) {
    if ((uVar1 & 0x204) == 0) {
      return -0x3fffff72;
    }
    if ((uVar1 & 0x102) != 0) {
      if ((uVar1 & 0x408) == 0) {
        return -0x3fffff6f;
      }
      if ((uVar1 & 0x810) != 0) {
        if ((uVar1 & 0x1020) != 0) {
          return param_1;
        }
        return -0x3fffff71;
      }
      return -0x3fffff6d;
    }
  }
  return -0x3fffff70;
}

