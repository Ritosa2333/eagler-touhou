/* Entry: 0x00452490; symbol: FUN_00452490; body bytes: 89 */

undefined1 * __cdecl FUN_00452490(undefined1 *param_1,uint param_2,undefined1 *param_3)

{
  char cVar1;
  
  cVar1 = FUN_00411170();
  if (cVar1 == '\0') {
    param_3 = (undefined1 *)FUN_00452400(param_1,param_2);
  }
  else {
    for (; param_2 != 0; param_2 = param_2 - 1) {
      *param_3 = *param_1;
      param_3 = param_3 + 1;
      param_1 = param_1 + 1;
    }
  }
  return param_3;
}

