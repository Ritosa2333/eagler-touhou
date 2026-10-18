/* Entry: 0x0040fdf0; symbol: FUN_0040fdf0; body bytes: 55 */

undefined2 * __cdecl FUN_0040fdf0(undefined2 *param_1,int param_2,undefined2 param_3)

{
  undefined2 *local_8;
  
  local_8 = param_1;
  for (; param_2 != 0; param_2 = param_2 + -1) {
    *local_8 = param_3;
    local_8 = local_8 + 1;
  }
  return param_1;
}

