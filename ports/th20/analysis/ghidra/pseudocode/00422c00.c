/* Entry: 0x00422c00; symbol: FUN_00422c00; body bytes: 36 */

uint __cdecl FUN_00422c00(uint param_1)

{
  param_1 = param_1 % 0x7fffffff;
  if (param_1 == 0) {
    param_1 = 1;
  }
  return param_1;
}

