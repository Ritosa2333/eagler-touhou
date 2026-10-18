/* Entry: 0x0040fda0; symbol: FUN_0040fda0; body bytes: 53 */

undefined1 * __cdecl FUN_0040fda0(undefined1 *param_1,int param_2,undefined1 param_3)

{
  undefined1 *local_8;
  
  local_8 = param_1;
  for (; param_2 != 0; param_2 = param_2 + -1) {
    *local_8 = param_3;
    local_8 = local_8 + 1;
  }
  return param_1;
}

