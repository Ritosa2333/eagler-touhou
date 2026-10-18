/* Entry: 0x004c8390; symbol: FUN_004c8390; body bytes: 122 */

undefined4 * __fastcall FUN_004c8390(undefined4 *param_1)

{
  FUN_004c85b0(param_1);
  *param_1 = &PTR_FUN_00571374;
  FUN_0048b5b0(param_1 + 0x1be);
  FUN_00448b40(param_1 + 0x1d2);
  param_1[0x34b] = 0;
  _memset(param_1 + 0x34c,0,0x800);
  _memset(param_1 + 0x54c,0,0x800);
  return param_1;
}

