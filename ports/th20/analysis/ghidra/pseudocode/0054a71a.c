/* Entry: 0x0054a71a; symbol: FUN_0054a71a; body bytes: 65 */

undefined1 __cdecl FUN_0054a71a(byte param_1,int param_2,uint param_3)

{
  bool bVar1;
  bool bVar2;
  
  if ((param_1 & 4) != 0) {
    return 1;
  }
  if ((param_1 & 1) != 0) {
    if ((param_1 & 2) == 0) {
      if (0x7fffffff < param_3) {
        return 1;
      }
      if (param_3 < 0x7fffffff) {
        return 0;
      }
      bVar1 = param_2 != -1;
      bVar2 = param_2 == -1;
    }
    else {
      if (0x80000000 < param_3) {
        return 1;
      }
      if (param_3 < 0x80000000) {
        return 0;
      }
      bVar1 = false;
      bVar2 = param_2 == 0;
    }
    if (!bVar1 && !bVar2) {
      return 1;
    }
  }
  return 0;
}

