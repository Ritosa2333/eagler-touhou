/* Entry: 0x004a3650; symbol: FUN_004a3650; body bytes: 119 */

undefined4 * __fastcall FUN_004a3650(undefined4 *param_1)

{
  *param_1 = &PTR_FUN_005703a0;
  param_1[1] = 0;
  param_1[2] = 0;
  _memset(param_1 + 3,0,0x100);
  _memset(param_1 + 0x43,0,0x100);
  FUN_004141c0(param_1 + 0x83);
  _String_val<>(param_1 + 0x87);
  return param_1;
}

