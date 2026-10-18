/* Entry: 0x004a3530; symbol: FUN_004a3530; body bytes: 71 */

undefined4 * __fastcall FUN_004a3530(undefined4 *param_1)

{
  *param_1 = 0xffffffff;
  param_1[1] = 0xffffffff;
  _memset(param_1 + 2,0,0x40);
  _memset(param_1 + 0x12,0,0x40);
  return param_1;
}

