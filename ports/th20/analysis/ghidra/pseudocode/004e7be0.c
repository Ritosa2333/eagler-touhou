/* Entry: 0x004e7be0; symbol: FUN_004e7be0; body bytes: 82 */

undefined1 * __cdecl FUN_004e7be0(undefined1 *param_1,undefined1 *param_2,undefined1 *param_3)

{
  char cVar1;
  
  cVar1 = FUN_00411170();
  if (cVar1 == '\0') {
    param_3 = (undefined1 *)FUN_004e7b00();
  }
  else {
    for (; param_1 != param_2; param_1 = param_1 + 1) {
      *param_3 = *param_1;
      param_3 = param_3 + 1;
    }
  }
  return param_3;
}

