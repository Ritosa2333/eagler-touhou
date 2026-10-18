/* Entry: 0x0041fcb0; symbol: FUN_0041fcb0; body bytes: 102 */

undefined4 * __fastcall FUN_0041fcb0(undefined4 *param_1)

{
  *param_1 = 0;
  param_1[1] = 0;
  param_1[2] = 0;
  param_1[3] = 0;
  FUN_0041f9b0(param_1 + 4);
  _memset(param_1 + 0xb4,0,0x100);
  param_1[0xf4] = 0;
  return param_1;
}

