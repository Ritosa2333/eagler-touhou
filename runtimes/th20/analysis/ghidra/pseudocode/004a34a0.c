/* Entry: 0x004a34a0; symbol: FUN_004a34a0; body bytes: 136 */

undefined4 * __fastcall FUN_004a34a0(undefined4 *param_1)

{
  *param_1 = 0;
  param_1[1] = 0;
  param_1[2] = 0;
  _memset(param_1 + 3,0,0x40);
  _memset(param_1 + 0x13,0,0x40);
  param_1[0x23] = 0;
  FUN_00422d90(param_1 + 0x24);
  param_1[0x28] = 0;
  param_1[0x29] = 0;
  return param_1;
}

