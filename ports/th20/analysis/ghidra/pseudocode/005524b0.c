/* Entry: 0x005524b0; symbol: FUN_005524b0; body bytes: 114 */

undefined4 __cdecl FUN_005524b0(int param_1,uint param_2)

{
  if (param_1 == 0) {
    if (param_2 == 0x7ff00000) {
      return 1;
    }
    if (param_2 == 0xfff00000) {
      return 2;
    }
  }
  if ((param_2 & 0x7ff80000) == 0x7ff80000) {
    return 3;
  }
  if (((param_2 & 0x7ff00000) == 0x7ff00000) &&
     ((param_1 != 0 || ((param_2 & 0x7fffffff) != 0x7ff00000)))) {
    return 4;
  }
  return 0;
}

